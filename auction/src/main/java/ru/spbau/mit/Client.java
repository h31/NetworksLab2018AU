package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.Closeable;
import java.io.EOFException;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import static ru.spbau.mit.Protocol.*;

import static ru.spbau.mit.Utils.*;

public class Client implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Client");
    private Socket socket = new Socket();
    private volatile boolean finish = false;

    public static void main(String[] args) {
        CommandLineParser parser = new DefaultParser();
        Options options = new Options();
        options.addOption("h", HOST_PARAMETER, true, HOST_PARAMETER);
        options.addRequiredOption("p", PORT_PARAMETER, true, PORT_PARAMETER);
        options.addOption("r", CLIENT_ROLE_PARAMETER, true, CLIENT_ROLE_PARAMETER);

        InetSocketAddress socketAddress;
        ClientRole clientRole;
        try {
            LOGGER.debug("Parsing options");
            CommandLine commandLine = parser.parse(options, args);
            socketAddress = socketAddressFromCommandLine(commandLine);
            String roleString = commandLine.getOptionValue(CLIENT_ROLE_PARAMETER,  ClientRole.PARTICIPANT.roleString());
            clientRole = ClientRole.valueOf(roleString.toUpperCase());
        } catch (ParseException e) {
            LOGGER.error("Failed to parse" + e);
            return;
        }

        // TODO put main loop flow into client's method.
        Scanner scanner = new Scanner(System.in);
        try (Client client = new Client(socketAddress)) {
            client.register(clientRole);
            LOGGER.info("Client registered successfully. Opening client commands' loop.");
            label: while (!client.finish) {
                // TODO command line requests.
                LOGGER.debug("Waiting for next command from CLI.");
                String command = scanner.nextLine().trim();
                switch (command) {
                    case EXIT_COMMAND:
                        break label;
                    case LIST_COMMAND:
                        LOGGER.info("Sending request on current lots list.");
                        List<Lot> lots = client.list();
                        System.out.println("Current lots:");
                        for (Lot lot : lots) {
                            String msg = String.format("lot #%d \"%s\" cost: %d", lot.getId(), lot.getName(), lot.getCost());
                            System.out.println(msg);
                        }
                        break;
                    case FINISH_COMMAND:
                        LOGGER.info("Sending finish request.");
                        client.finish();
                        break;
                    default:
                        LOGGER.error("Unknown client command: " + command);
                        break;
                }
            }
        } catch (IOException | ProtocolException e) {
            LOGGER.error("Client error: " + e);
        }
    }

    public Client(InetSocketAddress serverAddress) throws IOException {
        socket.connect(serverAddress);
    }

    public void register(ClientRole role) throws IOException, ProtocolException {
        send(clientInitRequest(role));
        String serverInitResponse = receive();
        checkServerOk(serverInitResponse);
    }

    public List<Lot> list() throws IOException, ProtocolException {
        final String request = CLIENT_LIST_REQUEST_HEADER;
        send(request);
        LOGGER.debug("send request to server: " + request);
        final String response = receive();
        LOGGER.debug("server response: " + response);

        String[] headerAndBody = response.split("\n\n");
        String header = headerAndBody[0];
        checkServerOk(header);
        String body = headerAndBody[1];
        Scanner scanner = new Scanner(body);

        List<Lot> result = new ArrayList<>();
        String firstLine = scanner.nextLine().trim();
        int lotsCount = Integer.parseInt(firstLine);
        for (int i = 0; i < lotsCount; i++) {
            String lotLine = scanner.nextLine();
            String[] idAndCostAndName = lotLine.split(" ");
            long id = Long.parseLong(idAndCostAndName[0]);
            long cost = Long.parseLong(idAndCostAndName[1]);
            StringBuilder nameBuilder = new StringBuilder();
            for (int j = 2; j < idAndCostAndName.length; j++) {
                nameBuilder.append(idAndCostAndName[j]);
                if (j + 1 < idAndCostAndName.length) {
                    nameBuilder.append(" ");
                }
            }
            String name = nameBuilder.toString();
            result.add(new Lot(id, name, cost));
        }
        return result;
    }

    public void finish() throws IOException, ProtocolException {
        send(CLIENT_FINISH_REQUEST_HEADER);
        String response = receive();
        if (response.equals(SERVER_OK_RESPONSE_HEADER)) {
            LOGGER.info("Auction finishing is acknowledged.");
            finish = true;
        } else {
            String message = response.split(HEADER_AND_BODY_DELIMITER)[1];
            LOGGER.info("Auction is not finished. Cause: " + message);
        }
    }

    @Override
    public void close() throws IOException {
        if (socket != null) {
            try {
                send(CLIENT_EXIT_REQUEST_HEADER);
                String response = receive();
                if (!response.equals(SERVER_OK_RESPONSE_HEADER)) {
                    LOGGER.warn("Bad response from server: " + response);
                }
            } catch (EOFException | SocketException e) {
                LOGGER.warn("Server socket is closed on other side: " + e);
            } catch (IOException e) {
                LOGGER.error("Error while closing server: " + e);
            }
            socket.close();
        }
    }

    private void send(String message) throws IOException {
        sendMessage(socket, message);
    }

    private String receive() throws IOException {
        return receiveMessage(socket);
    }
}
