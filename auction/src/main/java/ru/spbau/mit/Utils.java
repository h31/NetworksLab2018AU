package ru.spbau.mit;

import org.apache.commons.cli.CommandLine;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Utils {
    public static final String HOST_PARAMETER = "host";
    public static final String PORT_PARAMETER = "port";
    public static final String CLIENT_ROLE_PARAMETER = "role";

    public static final String HELP_COMMAND = "help";
    public static final String EXIT_COMMAND = "exit";
    public static final String LIST_COMMAND = "list";
    public static final String FINISH_COMMAND = "finish";
    public static final String ADD_COMMAND = "add";
    public static final String BET_COMMAND = "bet";

//    public static void TODO(String msg) {
//        throw new NotImplementedException(msg);
//    }
//
//    public static void TODO() {
//        TODO("TODO");
//    }

    public static InetSocketAddress socketAddressFromCommandLine(CommandLine commandLine) {
        String hostname = commandLine.getOptionValue(HOST_PARAMETER, "localhost");
        String portString = commandLine.getOptionValue(PORT_PARAMETER);
        int port = Integer.parseInt(portString);
        return new InetSocketAddress(hostname, port);
    }

    public static void sendMessage(Socket socket, String request) throws IOException {
        new DataOutputStream(socket.getOutputStream()).writeUTF(request);
    }

    public static String receiveMessage(Socket socket) throws IOException {
        return new DataInputStream(socket.getInputStream()).readUTF();
    }

    public static String buildStringFromSuffix(String[] strings, int start) {
        StringBuilder builder = new StringBuilder();
        for (int i = start; i < strings.length; i++) {
            builder.append(strings[i]).append(" ");
        }
        return builder.toString();
    }
}
