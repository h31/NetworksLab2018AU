package server;

import com.sun.istack.internal.NotNull;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.TimeUnit;

public class ConnectionHandler implements Runnable {

    @NotNull
    private final ServerSocket serverSocket;
    @NotNull
    private final LongTaskPool longTaskPool;
    private int longTaskLimit;


    public ConnectionHandler(int longTaskLimit, ServerSocket serverSocket, LongTaskPool longTaskPool) {
        this.longTaskLimit = longTaskLimit;
        this.serverSocket = serverSocket;
        this.longTaskPool = longTaskPool;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            try {
                final Socket accept = serverSocket.accept();
                accept.setSoTimeout((int) TimeUnit.SECONDS.toMillis(120));
                final Thread task = new Thread(new TaskHandler(longTaskLimit, accept, longTaskPool));
                task.setDaemon(true);
                task.start();
            } catch (IOException ignored) {
            }
        }
    }
}
