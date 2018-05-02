package server;

import com.sun.istack.internal.NotNull;

import java.io.IOException;
import java.net.ServerSocket;

public class Server {

    private ServerSocket serverSocket;
    private short port;
    private Thread connectionAccepter;
    private int longTaskLimit = Integer.MAX_VALUE;

    @NotNull
    private final LongTaskPool longTaskPool = new LongTaskPool();

    public Server(short port) {
        this.port = port;
    }

    public Server(short port, int longTaskLimit) {
        this.port = port;
        this.longTaskLimit = longTaskLimit;
    }

    public void start() throws IOException {
        serverSocket = new ServerSocket(port, 1000);
        connectionAccepter = new Thread(new ConnectionHandler(longTaskLimit, serverSocket, longTaskPool));
        connectionAccepter.start();
    }

    public void stop() throws IOException {
        connectionAccepter.interrupt();
        serverSocket.close();
    }

    public void join() throws InterruptedException {
        connectionAccepter.join();
    }
}
