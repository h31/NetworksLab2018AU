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

        Scanner scanner = new Scanner(System.in);
        try (Server server = new Server(socketAddress)) {
            while (true) {
                // TODO command line requests.
                String line = scanner.nextLine();
                final boolean hasToFinish = line.trim().equals(EXIT_COMMAND);
                if (hasToFinish) {
                    break;
                }
            }
        } catch (IOException e) {
            LOGGER.error("Serving error: " + e);
        }
    }

    public Server(InetSocketAddress address) throws IOException {
        this.serverSocket = new ServerSocket();
        serverSocket.bind(address);
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

        ServerSession(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                init();
                log(Level.DEBUG, "Initialization finished.");
                while (!finish) {
                    String clientRequest = receive();
                    log(Level.DEBUG, "Received request: " + clientRequest);
                    String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientRequest);
                    if (headerAndBody[0].equals(CLIENT_LIST_REQUEST_HEADER)) {
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
                            for (Lot lot : lotsCopy) {
                                responseBuilder.append(String.format("%d %d %s\n", lot.getId(), lot.getCost(), lot.getName()));
                            }
                            response = responseBuilder.toString();
                        } catch (Throwable e) {
                            response = SERVER_ERROR_RESPONSE_HEADER + HEADER_AND_BODY_DELIMITER + e.getMessage();
                        }
                        log(Level.DEBUG, "Sending response: " + response);
                        send(response);
                    }
                }
            } catch (IOException e) {
                LOGGER.error("ServerSession run error: " + e);
            }
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
                log(Level.DEBUG, ": Client wants to be come an admin");
                if (assignAdmin()) {
                    response = SERVER_OK_RESPONSE_HEADER;
                } else {
                    response = CURRENT_PROTOCOL + " 200 FAIL\n\nrole is busy";
                }
            } else if (!roleString.equals(ClientRole.PARTICIPANT.roleString())) {
                response = CURRENT_PROTOCOL + " 200 FAIL\n\nrole is wrong";
            } else {
                isAdmin = false;
                response = SERVER_OK_RESPONSE_HEADER;
            }
            log(Level.DEBUG, "sending response: " + response);
            sendMessage(socket, response);
        }
    }

}
