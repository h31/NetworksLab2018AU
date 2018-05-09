package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.Closeable;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;

import static ru.spbau.mit.Utils.HOST_PARAMETER;
import static ru.spbau.mit.Utils.PORT_PARAMETER;
import static ru.spbau.mit.Utils.socketAddressFromCommandLine;

public class Client implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Client");
    Socket socket = new Socket();

    public static void main(String[] args) {
        CommandLineParser parser = new DefaultParser();
        Options options = new Options();
        options.addOption("h", HOST_PARAMETER, true, HOST_PARAMETER);
        options.addRequiredOption("p", PORT_PARAMETER, true, PORT_PARAMETER);

        InetSocketAddress socketAddress;
        try {
            LOGGER.info("Parsing options: " + options);
            CommandLine commandLine = parser.parse(options, args);
            socketAddress = socketAddressFromCommandLine(commandLine);
        } catch (ParseException e) {
            LOGGER.error("Failed to parse" + e);
            return;
        }

        try (Client client = new Client(socketAddress)) {
            while (true) {
                // TODO command line requests.
                boolean hasToFinish = true;
                if (hasToFinish) {
                    break;
                }
            }
        } catch (IOException e) {
            LOGGER.error("Client error: " + e);
        }
    }

    public Client(InetSocketAddress serverAddress) throws IOException {
        socket.connect(serverAddress);
    }

    @Override
    public void close() throws IOException {
        socket.close();
    }
}
