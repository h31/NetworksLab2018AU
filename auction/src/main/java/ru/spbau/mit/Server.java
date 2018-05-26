package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.omg.CORBA.PRIVATE_MEMBER;

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
    private static final String HELP_STRING = "Help.\n"
            + "Commands: \n"
            + "- " + EXIT_COMMAND + ": Ending auction and exiting server.\n"
            + "- " + LIST_COMMAND + ": list current lots and their states.\n"
    ;
    private static final Logger LOGGER = LogManager.getLogger("Server");
    private Future acceptorThread = null;
    private final ExecutorService threadPool = Executors.newCachedThreadPool();
    private ServerSocket serverSocket;
    private ServerSession adminClientSession = null;
    private final Object serverStateMonitor = new Object();
    private final Map<Long, Lot> lotsMap = new HashMap<>();

    ServerState State = ServerState.NONE;

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

        System.out.println(HELP_STRING);

        try (Server server = new Server(socketAddress)) {
            server.mainLoop();
        } catch (IOException | ServerException e) {
            LOGGER.error("Serving error: " + e);
        }
    }

    public Server(InetSocketAddress address) throws IOException, ServerException {
        this.serverSocket = new ServerSocket();
        serverSocket.bind(address);
        addLot(new Lot(10));
        addLot(new Lot(1234567890123L));
        serve();
    }

    public void serve() {
        acceptorThread = threadPool.submit(() -> {
            try {
                while (State != ServerState.FINISHING) {
                    Socket socket;
                    try {
                        socket = serverSocket.accept();
                    } catch (SocketException e) {
                        if (State == ServerState.FINISHING) {
                            break;
                        } else {
                            throw e;
                        }
                    }
                    ServerSession clientSession = new ServerSession(this, socket);
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
        State = ServerState.RUNNING;
        LOGGER.info("server is up and running");
        label:
        while (State != ServerState.FINISHING) {
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
                                // TODO get verbose soldState for server.
                                String lotString = getLotString(lot, this, ", ");
                                System.out.println(lotString);
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

    @Override
    public void close() throws IOException {
        State = ServerState.FINISHING;
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

    void addLot(Lot lot) throws ServerException {
        synchronized (lotsMap) {
            if (lotsMap.containsKey(lot.getId())) {
                throw new ServerException("lot with id#" + lot.getId() + " already in lotsMap.");
            }
            lotsMap.put(lot.getId(), lot);
        }
    }

    void raiseLot(long lotId, long newCost, Object whoseBet) throws ServerException {
        synchronized (lotsMap) {
            if (!lotsMap.containsKey(lotId)) {
                throw new ServerException("lot with id#" + lotId + " does not exist.");
            }
            Lot lot = lotsMap.get(lotId);
            if (lot.getCost() >= newCost) {
                throw new ServerException("lot with id#" + lotId + " costs more or equal to " + newCost);
            }
            lot.setCost(newCost);
            lot.setOwner(whoseBet);
        }
    }

    List<Lot> list() throws ProtocolException {
        List<Lot> lotsCopy = new ArrayList<>();
        synchronized (lotsMap) {
            // creating lots snapshot.
            for (Lot lot : lotsMap.values()) {
                lotsCopy.add(new Lot(lot));
            }
        }
        return lotsCopy;
    }

    String getLotString(Lot lot, Object whosAsking, String delimiter) {
        ServerSession lotOwner = (ServerSession) lot.getOwner();
        String soldState;
        if (State == ServerState.RUNNING) {
            String soldStateSuffix = "";
            if (lotOwner == whosAsking) {
                soldStateSuffix = ", assumed to you";
            }
            if (Server.class.isInstance(lotOwner)) {
                soldStateSuffix = ", assumed to " + lotOwner;
            }
            soldState = "not sold yet" + soldStateSuffix;
        } else {
            if (lotOwner == null) {
                soldState = "not sold";
            } else if (whosAsking == lotOwner) {
                soldState = "sold to you";
            } else if (whosAsking == this) {
                soldState = "sold out to: " + lotOwner;
            } else {
                soldState = "sold out";
            }
        }
        return String.format(String.join(delimiter, Arrays.asList("%d", "%d", "%s", "%s")), lot.getId(), lot.getCost(), lot.getName(), soldState);
    }

    /**
     * @return true on succeed, false on failure.
     */
    boolean assignAdmin(ServerSession newAdminSession) {
        synchronized (serverStateMonitor) {
            if (adminClientSession == null) {
                adminClientSession = newAdminSession;
                return true;
            } else {
                return false;
            }
        }
    }

    void deassignAdmin(ServerSession oldAdminSession) throws Server.ServerException {
        synchronized (serverStateMonitor) {
            if (adminClientSession != oldAdminSession) {
                throw new Server.ServerException("adminClientSession is expected to be this");
            }
            adminClientSession = null;
        }
    }

    enum ServerState {
        NONE,
        RUNNING,
        AUCTION_FINISHED,
        FINISHING
    }

    static class ServerException extends Exception {
        ServerException(String message) {
            super(message);
        }
    }
}
