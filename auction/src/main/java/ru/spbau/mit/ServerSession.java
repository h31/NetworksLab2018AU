package ru.spbau.mit;

import org.apache.logging.log4j.Level;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.net.Socket;
import java.util.List;
import java.util.function.Supplier;

import static ru.spbau.mit.Protocol.*;
import static ru.spbau.mit.Protocol.LINE_DELIMITER;
import static ru.spbau.mit.Protocol.serverFailResponse;
import static ru.spbau.mit.Utils.buildStringFromSuffix;
import static ru.spbau.mit.Utils.receiveMessage;
import static ru.spbau.mit.Utils.sendMessage;

class ServerSession implements Runnable {
    private static final Logger LOGGER = LogManager.getLogger("ServerSession");
    private Socket socket;
    private boolean isAdmin;
    private boolean finishSession = false;
    private Server server;

    ServerSession(Server server, Socket socket) {
        this.server = server;
        this.socket = socket;
    }

    @Override
    public void run() {
        try {
            try {
                init();
                log(Level.DEBUG, "Initialization finished.");
            } catch (Server.ServerException e) {
                log(Level.ERROR, "Initialization failed.");
                return;
            }

            while (server.State != Server.ServerState.FINISHING && !finishSession) {
                String clientRequest = receive();
                log(Level.DEBUG, "Received request: " + clientRequest);
                if (server.State == Server.ServerState.FINISHING || finishSession) {
                    send(serverFailResponse("Server is not available", true));
                    break;
                }
                String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientRequest);
                String header = headerAndBody[0];

                // in order to avoid IndexOutOfBoundsException
                Supplier<String[]> bodyPartsGetter = () -> {
                    String body = headerAndBody[1];
                    if (body.split("\n").length > 1) {
                        log(Level.WARN, "Body contains too many lines.");
                    }
                    return body.split(" ");
                };

                String name;
                long cost, id;
                String response = null;

                try {
                    switch (header) {
                        case CLIENT_LIST_REQUEST_HEADER: {
                            response = listResponse();
                            break;
                        }
                        case CLIENT_EXIT_REQUEST_HEADER: {
                            log(Level.INFO, "Closing client initiated.");
                            finishSession = true;
                            // We need to deassign ourselves in order to give admin possibility from other client.
                            if (isAdmin) {
                                server.deassignAdmin(this);
                            }
                            response = SERVER_OK_RESPONSE_HEADER;
                            break;
                        }
                        case CLIENT_FINISH_REQUEST_HEADER: {
                            // finishing auction request.
                            if (!isAdmin) {
                                response = serverFailResponse("Client is not authorized for this operation", false);
                            } else {
                                server.State = Server.ServerState.AUCTION_FINISHED;
                                response = SERVER_OK_RESPONSE_HEADER;
                                log(Level.INFO, "Auction is finished.");
                            }
                            break;
                        }
                        case CLIENT_ADD_REQUEST_HEADER: {
                            if (!isAdmin) {
                                response = serverFailResponse("Wrong role for adding lots. ", false);
                            } else {
                                cost = Long.parseLong(bodyPartsGetter.get()[0]);
                                name = buildStringFromSuffix(bodyPartsGetter.get(), 1);
                                Lot lot = new Lot(name, cost);
                                server.addLot(lot);
                                response = SERVER_OK_RESPONSE_HEADER + HEADER_AND_BODY_DELIMITER + lot.getId();
                            }
                            break;
                        }
                        case CLIENT_BET_REQUEST_HEADER: {
                            if (isAdmin) {
                                response = serverFailResponse("Wrong role for betting lots. ", false);
                            } else {
                                cost = Long.parseLong(bodyPartsGetter.get()[0]);
                                id = Long.parseLong(bodyPartsGetter.get()[1]);
                                server.raiseLot(id, cost, this);
                                response = SERVER_OK_RESPONSE_HEADER;
                            }
                            break;
                        }
                        default:
                            response = serverFailResponse("Unexpected request", false);
                            log(Level.ERROR, "Unexpected request: " + clientRequest);
                            break;
                    }
                } catch (Server.ServerException e) {
                    response = serverFailResponse(e.getMessage(), true);
                } finally {
                    if (response != null) {
                        send(response);
                    }
                }
            }
        } catch (Throwable t) {
            String errorMessage = "ServerSession run error: " + t.getMessage();
            log(Level.ERROR, errorMessage);
            try {
                send(serverFailResponse(errorMessage, true));
            } catch (IOException e) {
                log(Level.ERROR, "ERROR while trying to close connection: " + e.getMessage());
            }
        }
        log(Level.DEBUG, "session finished");
    }

    private void send(String response) throws IOException {
        log(Level.DEBUG, "Sending response: " + response);
        sendMessage(socket, response);
    }

    private String receive() throws IOException {
        return receiveMessage(socket);
    }

    private void log(Level logLevel, String msg) {
        String logMessage = "session#" + this.toString() + ": " + msg;
        LOGGER.log(logLevel, logMessage);
    }

    private void init() throws IOException, Server.ServerException {
        String clientInitialRequest = receiveMessage(socket);
        String[] headerAndBody = Protocol.splitOnHeaderAndBody(clientInitialRequest);
        // TODO put into a separate function.
        String headerString = headerAndBody[0];
        if (!headerString.equals(CLIENT_INIT_REQUEST_HEADER)) {
            throw new Server.ServerException("Unexpected request from client.");
        }
        String roleString = headerAndBody[1];
        // TODO other http codes on failures??
        // TODO put response into Protocol's inner class.
        String response;
        if (roleString.equals(ClientRole.ADMIN.roleString())) {
            if (server.assignAdmin(this)) {
                isAdmin = true;
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
            List<Lot> lotsCopy = server.list();
            StringBuilder responseBuilder = new StringBuilder(SERVER_OK_RESPONSE_HEADER);
            responseBuilder.append(Protocol.HEADER_AND_BODY_DELIMITER);
            responseBuilder.append(lotsCopy.size()).append(LINE_DELIMITER);
            for (Lot lot : lotsCopy) {
                String lotString = server.getLotString(lot, this, Protocol.INLINE_DELIMITER);
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