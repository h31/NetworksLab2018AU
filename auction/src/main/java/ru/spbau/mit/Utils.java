package ru.spbau.mit;

import org.apache.commons.cli.CommandLine;

import java.net.InetSocketAddress;

public class Utils {
    public static final String HOST_PARAMETER = "host";
    public static final String PORT_PARAMETER = "port";

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

}
