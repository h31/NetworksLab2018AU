package ru.spbau.mit;

import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.StringTokenizer;
import java.util.concurrent.*;
import static ru.spbau.mit.Utils.*;

public class Server implements Closeable {
    private static final Logger LOGGER = LogManager.getLogger("Server");
    private final ExecutorService threadPool = Executors.newCachedThreadPool();
//    private final List<Future> workerFutures = new ArrayList<>();
    private volatile boolean finish = false;
    private ServerSocket serverSocket;

    private final List<Lot> lots = new ArrayList<>();

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

        Scanner scanner = new Scanner(System.in);
        try (Server server = new Server(socketAddress)) {
            server.serve();
            while (true) {
                // TODO command line requests.
                String line = scanner.nextLine();
//                line.split()

                boolean hasToFinish = true;
                if (hasToFinish) {
                    break;
                }
            }
        } catch (IOException e) {
            LOGGER.error("Serving error: " + e);
        }
    }

    public Server(InetSocketAddress address) throws IOException {
        this.serverSocket = new ServerSocket();
        serverSocket.bind(address);
    }

    public void serve() {
        threadPool.submit(() -> {
            try {
                while (!finish) {
                    Socket socket = serverSocket.accept();
                    threadPool.submit(new ServerSession(socket));
                }
            } catch (IOException e) {
                LOGGER.error("Error while accepting sockets." + e);
            }
        });
    }

    public void addLot(Lot lot) {
        synchronized (lots) {
            lots.add(lot);
        }
    }

    @Override
    public void close() throws IOException {
        finish = true;
        if (serverSocket != null) {
            serverSocket.close();
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

    private class ServerSession implements Runnable {
        private Socket socket;

        ServerSession(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                OutputStream outputStream = socket.getOutputStream();
                InputStream inputStream = socket.getInputStream();

            } catch (IOException e) {
                LOGGER.error("ServerSession run error: " + e);
            }
        }
    }
}
