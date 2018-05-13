package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.Level;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.*;
import java.util.concurrent.*;

import static ru.spbau.mit.Protocol.*;
import static ru.spbau.mit.Utils.*;


public class Server implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Server");
    private Future acceptorThread = null;
    private final ExecutorService threadPool = Executors.newCachedThreadPool();
//    private final List<Future> workerFutures = new ArrayList<>();
    private volatile boolean finish = false;
    private ServerSocket serverSocket;
    private ServerSession adminClientSession = null;
    private final Object serverStateMonitor = new Object();

    private final List<Lot> lots = new ArrayList<>();

    public static void main(String[] args) {
        CommandLineParser parser = new DefaultParser();
        Options options = new Options();
        options.addOption("h", HOST_PARAMETER, true, HOST_PARAMETER);
        options.addRequiredOption("p", PORT_PARAMETER, true, PORT_PARAMETER);

        InetSocketAddress socketAddress;

        try {
            LOGGER.debug("Parsing options");
            CommandLine commandLine = parser.parse(options, args);
            socketAddress = socketAddressFromCommandLine(commandLine);
        } catch (ParseException e) {
            LOGGER.error("Failed to parse: " + e);
            return;
        }


        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        try (Server server = new Server(socketAddress)) {
            LOGGER.info("server is up and running");
            while (!server.finish) {
                // TODO command line requests.
                if (br.ready()) {
                    String line = br.readLine();
                    final boolean hasToFinish = line.trim().equals(EXIT_COMMAND);
                    if (hasToFinish) {
                        break;
                    }
                }
            }
        } catch (IOException e) {
            LOGGER.error("Serving error: " + e);
        }
    }

    public Server(InetSocketAddress address) throws IOException {
        this.serverSocket = new ServerSocket();
        serverSocket.bind(address);
        lots.add(new Lot(10));
        lots.add(new Lot(1234567890123L));
        serve();
    }

    public void serve() {
        acceptorThread = threadPool.submit(() -> {
            try {
                while (!finish) {
                    Socket socket;
                    try {
                        socket = serverSocket.accept();
                    } catch (SocketException e) {
                        if (finish) {
                            break;
                        } else {
                            throw e;
                        }
                    }
                    ServerSession clientSession = new ServerSession(socket);
                    LOGGER.debug("socket accepted for session: " + clientSession);
                    threadPool.submit(clientSession);
                }
            } catch (IOException e) {
                LOGGER.error("Error while accepting sockets: " + e);
            }
        });
    }

    public void addLot(Lot lot) {
        synchronized (lots) {
            lots.add(lot);
        }
    }

    @Override
    public void close() throws IOException {
        finish = true;
        if (serverSocket != null) {
            serverSocket.close();
        }
        try {
            acceptorThread.get();
        } catch (InterruptedException | ExecutionException e) {
            LOGGER.error("Error while waiting for acceptor thread to finish: " + e);
        }

        this.threadPool.shutdown();
        LOGGER.info("Shutting down server...");
        try {
        for (int secondsPassed = 0; ; ++secondsPassed) {
                boolean isShutdown = this.threadPool.awaitTermination(1, TimeUnit.SECONDS);
                if (isShutdown) {
                    break;
                }
                LOGGER.warn("Shutting down. Seconds passed: " + secondsPassed);
            }
        } catch (InterruptedException e) {
            LOGGER.error("Interrupted while waiting for shutting down: " + e);
        }
    }

    private class ServerSession implements Runnable {
        private Socket socket;
        private boolean isAdmin;
        private boolean finishSession = false;

        ServerSession(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                init();
                log(Level.DEBUG, "Initialization finished.");
                while (!finish && !finishSession) {
                    String clientRequest = receive();
                    log(Level.DEBUG, "Received request: " + clientRequest);
                    if (finish || finishSession) {
                        send(serverFailResponse("Server is not available", true));
                        break;
                    }
                    String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientRequest);
                    String header = headerAndBody[0];
                    switch (header) {
                        case CLIENT_LIST_REQUEST_HEADER: {
                            String response;
                            try {
                                List<Lot> lotsCopy = new ArrayList<>();
                                synchronized (lots) {
                                    // creating lots snapshot.
                                    for (Lot lot : lots) {
                                        lotsCopy.add(new Lot(lot));
                                    }
                                }
                                StringBuilder responseBuilder = new StringBuilder(SERVER_OK_RESPONSE_HEADER);
                                responseBuilder.append(Protocol.HEADER_AND_BODY_DELIMITER);
                                responseBuilder.append(lotsCopy.size()).append("\n");
                                for (Lot lot : lotsCopy) {
                                    responseBuilder.append(String.format("%d %d %s\n", lot.getId(), lot.getCost(), lot.getName()));
                                }
                                response = responseBuilder.toString();
                            } catch (Throwable e) {
                                response = serverFailResponse(e.getMessage(), true);
                            }
                            log(Level.DEBUG, "Sending response: " + response);
                            send(response);
                            break;
                        }
                        case CLIENT_EXIT_REQUEST_HEADER:
                            log(Level.INFO, "Closing client initiated.");
                            send(SERVER_OK_RESPONSE_HEADER);
                            finishSession = true;
                            deassignAdmin();
                            break;
                        case CLIENT_FINISH_REQUEST_HEADER: {
                            // finishing auction request.
                            String response;
                            if (!isAdmin) {
                                response = serverFailResponse("Client is not authorized for this operation", false);
                            } else {
                                finish = true;
                                response = SERVER_OK_RESPONSE_HEADER;
                                LOGGER.info("Auction is finished.");
                            }
                            send(response);
                            break;
                        }
                    }
                }
            } catch (IOException e) {
                LOGGER.error("ServerSession run error: " + e);
            }
            log(Level.DEBUG, "session finished");
        }

        private void send(String response) throws IOException {
            sendMessage(socket, response);
        }

        private String receive() throws IOException {
            return receiveMessage(socket);
        }

        /**
         *
         * @return true on succeed, false on failure.
         */
        private boolean assignAdmin() {
            isAdmin = true;
            synchronized (serverStateMonitor) {
                if (adminClientSession == null) {
                    adminClientSession = this;
                    return true;
                } else {
                    return false;
                }
            }
        }

        private void deassignAdmin() {
            if (isAdmin) {
                synchronized (serverStateMonitor) {
                    if (adminClientSession != this) {
                        throw new RuntimeException("adminClientSession is expected to be this");
                    }
                    adminClientSession = null;
                }
            }
        }

        private void log(Level logLevel, String msg) {
            String logMessage = "session#" + this.toString() + ": " + msg;
            LOGGER.log(logLevel, logMessage);
        }

        private void init() throws IOException {
            String clientInitialRequest = receiveMessage(socket);
            String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientInitialRequest);
            // TODO put into a separate function.
            String headerString = headerAndBody[0];
            if (!headerString.equals(CLIENT_INIT_REQUEST_HEADER)) {
                throw new RuntimeException("Unexpected request from client.");
            }
            String roleString = headerAndBody[1];
            // TODO other http codes on failures??
            // TODO put response into Protocol's inner class.
            String response;
            if (roleString.equals(ClientRole.ADMIN.roleString())) {
                if (assignAdmin()) {
                    log(Level.DEBUG, "Client becomes an admin: OK");
                    response = SERVER_OK_RESPONSE_HEADER;
                } else {
                    log(Level.DEBUG, "Client becomes an admin but role is busy");
                    response = serverFailResponse("role is busy", false);
                }
            } else if (!roleString.equals(ClientRole.PARTICIPANT.roleString())) {
                response = serverFailResponse("role is wrong", false);
            } else {
                isAdmin = false;
                response = SERVER_OK_RESPONSE_HEADER;
            }
            log(Level.DEBUG, "sending response: " + response);
            sendMessage(socket, response);
        }
    }

}
