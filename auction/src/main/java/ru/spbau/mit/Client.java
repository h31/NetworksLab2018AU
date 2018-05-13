package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.Closeable;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;
import static ru.spbau.mit.Protocol.*;

import static ru.spbau.mit.Utils.*;

public class Client implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Client");
    Socket socket = new Socket();

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
                String line = scanner.nextLine();
                final boolean hasToFinish = line.trim().equals(EXIT_COMMAND);
                if (hasToFinish) {
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
        sendRequest(socket, clientInitRequest(role));
        String serverInitResponse = receiveRequest(socket);
        checkServerOk(serverInitResponse);
    }

    @Override
    public void close() throws IOException {
        socket.close();
    }
}
