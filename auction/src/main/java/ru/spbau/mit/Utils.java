package ru.spbau.mit;

import org.apache.commons.cli.CommandLine;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Utils {
    public static final String HOST_PARAMETER = "host";
    public static final String PORT_PARAMETER = "port";
    public static final String CLIENT_ROLE_PARAMETER = "role";

    public static final String EXIT_COMMAND = "exit";

    public static void TODO(String msg) {
        throw new NotImplementedException(msg);
    }

    public static void TODO() {
        TODO("TODO");
    }

    public static InetSocketAddress socketAddressFromCommandLine(CommandLine commandLine) {
        String hostname = commandLine.getOptionValue(HOST_PARAMETER, "localhost");
        String portString = commandLine.getOptionValue(PORT_PARAMETER);
        int port = Integer.parseInt(portString);
        return new InetSocketAddress(hostname, port);
    }

    public static void sendRequest(Socket socket, String request) throws IOException {
        new DataOutputStream(socket.getOutputStream()).writeUTF(request);
    }

    public static String receiveRequest(Socket socket) throws IOException {
        return new DataInputStream(socket.getInputStream()).readUTF();
    }
}
