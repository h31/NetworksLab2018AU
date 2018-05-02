package server;

import com.sun.istack.internal.NotNull;
import server.pages.*;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.StringTokenizer;

public class TaskHandler implements Runnable {

    private int longTaskLimit;
    @NotNull
    private final Socket client;
    @NotNull
    private final BufferedReader inClient;
    @NotNull
    private final DataOutputStream outClient;
    @NotNull
    private final LongTaskPool longTaskPool;

    public TaskHandler(int longTaskLimit, Socket client, LongTaskPool longTaskPool) throws IOException {
        this.longTaskLimit = longTaskLimit;
        this.client = client;
        inClient = new BufferedReader(new InputStreamReader(client.getInputStream()));
        outClient = new DataOutputStream(client.getOutputStream());
        this.longTaskPool = longTaskPool;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            String connection = "close";
            try {
                System.out.println("The Client " + client.getInetAddress() + ":" + client.getPort() + " is connected");

                final String headerLine = inClient.readLine();
                if (headerLine == null) {
                    return;
                }

                final StringTokenizer tokenizer = new StringTokenizer(headerLine);
                final String httpMethod = tokenizer.nextToken();
                final String httpQueryString = tokenizer.nextToken();

                System.out.println("The HTTP request string is ....");
                String requestString = headerLine;

                while (inClient.ready()) {
                    requestString = inClient.readLine();
                    String[] split = requestString.split(":");

                    if (split.length != 2) {
                        continue;
                    }

                    if (split[0].equals("Connection")) {
                        connection = split[1].trim();
                    }
                }

                if (!httpMethod.equals("GET")) {
                    return;
                }

                if (httpQueryString.equals("/")) {
                    outClient.writeBytes(new HomePage(connection).toString());
                } else if (httpQueryString.startsWith("/fast.html?")) {
                    outClient.writeBytes(new FastCalculationPage(connection, httpQueryString).toString());
                } else if (httpQueryString.startsWith("/long.html?")) {
                    outClient.writeBytes(new LongCalculationIdentifierPage(longTaskLimit, connection, httpQueryString, longTaskPool).toString());
                } else if (httpQueryString.startsWith("/task.html?")) {
                    outClient.writeBytes(new LongCalculationPage(connection, httpQueryString, longTaskPool).toString());
                } else {
                    outClient.writeBytes(new $404Page(connection).toString());
                }
            } catch (Exception e) {
                return;
            }
            if (connection.equals("close")) {
                return;
            }
        }
    }
}
