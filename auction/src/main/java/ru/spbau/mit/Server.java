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
//    private volatile boolean finish = false;
    private ServerSocket serverSocket;
    private ServerSession adminClientSession = null;
    private final Object serverStateMonitor = new Object();
    private ServerState serverState = ServerState.NONE;
//    private final List<Lot> lots = new ArrayList<>();
    private final Map<Long, Lot> lotsMap = new HashMap<>();

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

        try (Server server = new Server(socketAddress)) {
            server.mainLoop();
        } catch (IOException e) {
            LOGGER.error("Serving error: " + e);
        }
    }

    public Server(InetSocketAddress address) throws IOException {
        this.serverSocket = new ServerSocket();
        serverSocket.bind(address);
        addLot(new Lot(10));
        addLot(new Lot(1234567890123L));
        serve();
    }

    public void serve() {
        acceptorThread = threadPool.submit(() -> {
            try {
                while (serverState != ServerState.FINISHING) {
                    Socket socket;
                    try {
                        socket = serverSocket.accept();
                    } catch (SocketException e) {
                        if (serverState == ServerState.FINISHING) {
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

    public void mainLoop() throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        serverState = ServerState.RUNNING;
        LOGGER.info("server is up and running");
        label:
        while (serverState != ServerState.FINISHING) {
            // TODO command line requests.
            if (br.ready()) {
                String command = br.readLine().trim();
                switch (command) {
                    case EXIT_COMMAND:
                        break label;
                    case LIST_COMMAND:
                        try {
                            List<Lot> lots = list();
                            for (Lot lot : lots) {
                                System.out.println(getLotString(lot, this));
                            }
                        } catch (ProtocolException e) {
                            LOGGER.error("Failed to list lots: " + e);
                        }
                        break;
                    default:
                        LOGGER.error("Unknown server command: " + command);
                        break;
                }
            }
        }
    }

    public void addLot(Lot lot) throws ServerException {
        synchronized (lotsMap) {
            if (lotsMap.containsKey(lot.getId())) {
                throw new ServerException("lot with id#" + lot.getId() + " already in lotsMap.");
            }
            lotsMap.put(lot.getId(), lot);
        }
    }

    public void raiseLot(long lotId, long newCost, Object whoseBet) throws ServerException {
        synchronized (lotsMap) {
            if (lotsMap.containsKey(lotId)) {
                throw new ServerException("lot with id#" + lotId + " does not exist.");
            }
            Lot lot = lotsMap.get(lotId);
            if (lot.getCost() <= newCost) {
                throw new ServerException("lot with id#" + lotId + " costs more or equal to " + newCost);
            }
            lot.setCost(newCost);
            lot.setOwner(whoseBet);
        }
    }

    @Override
    public void close() throws IOException {
        serverState = ServerState.FINISHING;
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

    private List<Lot> list() throws ProtocolException {
        List<Lot> lotsCopy = new ArrayList<>();
        synchronized (lotsMap) {
            // creating lots snapshot.
            for (Lot lot : lotsMap.values()) {
                lotsCopy.add(new Lot(lot));
            }
        }
        return lotsCopy;
    }

    private String getLotString(Lot lot, Object whosAsking) {
        String soldState = null;
        if (serverState == ServerState.RUNNING) {
            soldState = "";
        } else {
            ServerSession lotOwner = (ServerSession) lot.getOwner();
            if (lotOwner == null) {
                soldState = "not sold";
            } else if (lotOwner == whosAsking) {
                soldState = "sold to you";
            } else if (whosAsking == this) {
                soldState = "sold";
            }
        }
        return String.format("%d %d %s %s", lot.getId(), lot.getCost(), lot.getName(), soldState);
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
                try {
                    init();
                    log(Level.DEBUG, "Initialization finished.");
                } catch (ServerException e) {
                    log(Level.ERROR, "Initialization failed.");
                    return;
                }

                while (serverState != ServerState.FINISHING && !finishSession) {
                    String clientRequest = receive();
                    log(Level.DEBUG, "Received request: " + clientRequest);
                    if (serverState == ServerState.FINISHING || finishSession) {
                        send(serverFailResponse("Server is not available", true));
                        break;
                    }
                    String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientRequest);
                    String header = headerAndBody[0];
                    String body = headerAndBody[1];
                    String[] bodyParts = body.split(" ");
                    String name;
                    long cost, id;
                    String response = null;

                    try {
                        switch (header) {
                            case CLIENT_LIST_REQUEST_HEADER: {
                                response = listResponse();
                                send(response);
                                break;
                            }
                            case CLIENT_EXIT_REQUEST_HEADER:
                                log(Level.INFO, "Closing client initiated.");
                                finishSession = true;
                                deassignAdmin();
                                response = SERVER_OK_RESPONSE_HEADER;
                                break;
                            case CLIENT_FINISH_REQUEST_HEADER:
                                // finishing auction request.
                                if (!isAdmin) {
                                    response = serverFailResponse("Client is not authorized for this operation", false);
                                } else {
                                    serverState = ServerState.AUCTION_FINISHED;
                                    response = SERVER_OK_RESPONSE_HEADER;
                                    log(Level.INFO, "Auction is finished.");
                                }
                                break;
                            case CLIENT_ADD_REQUEST_HEADER:
                                if (!isAdmin) {
                                    response = serverFailResponse("Wrong role for adding lots. ", false);
                                } else {
                                    cost = Long.parseLong(bodyParts[0]);
                                    name = buildStringFromSuffix(bodyParts, 1);
                                    Lot lot = new Lot(name, cost);
                                    addLot(lot);
                                    response = SERVER_OK_RESPONSE_HEADER + HEADER_AND_BODY_DELIMITER + lot.getId();
                                }
                                break;
                            case CLIENT_BET_REQUEST_HEADER:
                                if (isAdmin) {
                                    response = serverFailResponse("Wrong role for betting lots. ", false);
                                } else {
                                    cost = Long.parseLong(bodyParts[0]);
                                    id = Long.parseLong(bodyParts[1]);
                                    raiseLot(id, cost, this);
                                    response = SERVER_OK_RESPONSE_HEADER;
                                }
                                break;
                            default:
                                response = serverFailResponse("Unexpected request", false);
                                LOGGER.error("Unexpected request: " + clientRequest);
                                break;
                        }
                    } catch (ServerException e) {
                        response = serverFailResponse(e.getMessage(), true);
                    } finally {
                        if (response != null) {
                            send(response);
                        }
                    }
                }
            } catch (IOException e) {
                LOGGER.error("ServerSession run error: " + e);
            }
            log(Level.DEBUG, "session finished");
        }

        private void send(String response) throws IOException {
            LOGGER.debug("Sending response: " + response);
            sendMessage(socket, response);
        }

        private String receive() throws IOException {
            return receiveMessage(socket);
        }

        /**
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

        private void deassignAdmin() throws ServerException {
            if (isAdmin) {
                synchronized (serverStateMonitor) {
                    if (adminClientSession != this) {
                        throw new ServerException("adminClientSession is expected to be this");
                    }
                    adminClientSession = null;
                }
            }
        }

        private void log(Level logLevel, String msg) {
            String logMessage = "session#" + this.toString() + ": " + msg;
            LOGGER.log(logLevel, logMessage);
        }

        private void init() throws IOException, ServerException {
            String clientInitialRequest = receiveMessage(socket);
            String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientInitialRequest);
            // TODO put into a separate function.
            String headerString = headerAndBody[0];
            if (!headerString.equals(CLIENT_INIT_REQUEST_HEADER)) {
                throw new ServerException("Unexpected request from client.");
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

        /**
         *
         * @return response.
         */
        private String listResponse() {
            String response;
            try {
                List<Lot> lotsCopy = list();
                StringBuilder responseBuilder = new StringBuilder(SERVER_OK_RESPONSE_HEADER);
                responseBuilder.append(Protocol.HEADER_AND_BODY_DELIMITER);
                responseBuilder.append(lotsCopy.size()).append(LINE_DELIMITER);
                for (Lot lot : lotsCopy) {
                    String lotString = getLotString(lot, this);
                    responseBuilder
                            .append(lotString)
                            .append(LINE_DELIMITER);
                }
                response = responseBuilder.toString();
            } catch (Throwable e) {
                response = serverFailResponse(e.getMessage(), true);
            }
            return response;
        }
    }

    private enum ServerState {
        NONE,
        RUNNING,
        AUCTION_FINISHED,
        FINISHING
    }

    private class ServerException extends Exception {
        ServerException(String message) {
            super(message);
        }
    }
}
