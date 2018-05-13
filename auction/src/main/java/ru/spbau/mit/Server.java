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
        private static final String okResponse =  CURRENT_PROTOCOL + " 200 OK";

        ServerSession(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                String clientRequest = receiveRequest(socket);
                String[] headerAndBody = clientRequest.split("\n\n");
                if (headerAndBody.length != 2) {
                    throw new RuntimeException("protocol");
                }
                // TODO put into a separate function.
                String headerString = headerAndBody[0];
                if (!headerString.equals("GET /this_client/role " + CURRENT_PROTOCOL)) {
                    throw new RuntimeException("Unexpected request from client.");
                }
                String roleString = headerAndBody[1];
                // TODO other http codes on failures??
                String response;
                if (roleString.equals(ClientRole.ADMIN.roleString())) {
                    log(Level.DEBUG, ": Client wants to be come an admin");
                    if (assignAdmin()) {
                        response = okResponse;
                    } else {
                        response = CURRENT_PROTOCOL + " 200 FAIL\n\nrole is busy";
                    }
                } else if (!roleString.equals(ClientRole.PARTICIPANT.roleString())) {
                    response = CURRENT_PROTOCOL + " 200 FAIL\n\nrole is wrong";
                } else {
                    isAdmin = false;
                    response = okResponse;
                }
                log(Level.DEBUG, "sending response: " + response);
                sendRequest(socket, response);
            } catch (IOException e) {
                LOGGER.error("ServerSession run error: " + e);
            }
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
    }

}
