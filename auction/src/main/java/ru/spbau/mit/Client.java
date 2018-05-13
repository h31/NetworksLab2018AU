package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.Closeable;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import static ru.spbau.mit.Protocol.*;

import static ru.spbau.mit.Utils.*;

public class Client implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Client");
    private Socket socket = new Socket();

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

        Scanner scanner = new Scanner(System.in);
        try (Client client = new Client(socketAddress)) {
            client.register(clientRole);
            while (true) {
                // TODO command line requests.
                String line = scanner.nextLine().trim();
                if (line.equals(EXIT_COMMAND)) {
                    break;
                } else if (line.equals(LIST_COMMAND)) {
                    LOGGER.info("Sending request on current lots list.");
                    List<Lot> lots = client.list();
                    System.out.println("Current lots");
                    for (Lot lot : lots) {
                        String msg = String.format("lot #%d \"%s\" cost: %d", lot.getId(), lot.getName(), lot.getCost());
                        System.out.println(msg);
                    }
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

    @Override
    public void close() throws IOException {
        if (socket != null) {
            send(CLIENT_EXIT_REQUEST_HEADER);
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
