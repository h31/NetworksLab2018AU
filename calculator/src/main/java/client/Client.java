package client;

import com.sun.istack.internal.NotNull;
import protocol.HttpRequest;
import protocol.HttpRequestBuilder;

import java.io.*;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.net.Socket;
import java.util.concurrent.TimeUnit;

public class Client {
    @NotNull
    private Socket socket;
    @NotNull
    private BufferedReader inClient;
    @NotNull
    private DataOutputStream outClient;

    public Client(@NotNull final String host, short port) throws IOException {
        socket = new Socket(host, port);
        socket.setSoTimeout((int) TimeUnit.SECONDS.toMillis(3));
        inClient = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        outClient = new DataOutputStream(socket.getOutputStream());
    }

    public void close() throws IOException {
        socket.close();
    }

    @NotNull
    public String sendBinaryOperation(@NotNull final String addition, @NotNull final BigInteger x, @NotNull final BigInteger y) throws IllegalAccessException, InstantiationException, InvocationTargetException, IOException {
        final HttpRequest request =
                new HttpRequestBuilder()
                        .setPage("fast.html")
                        .setConnection("keep-alive")
                        .setHost(socket.getInetAddress().getHostName() + ":" + socket.getPort())
                        .setVariable("type", addition)
                        .setVariable("x", x.toString())
                        .setVariable("y", y.toString())
                        .build();

        return sendRequest(request);
    }

    private void reconnect() throws IOException {
        socket = new Socket(socket.getInetAddress(), socket.getPort());
        socket.setSoTimeout((int) TimeUnit.SECONDS.toMillis(1));
        inClient = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        outClient = new DataOutputStream(socket.getOutputStream());
    }

    @NotNull
    public String sendUnaryOperation(@NotNull final String factorial, @NotNull final BigInteger x) throws IllegalAccessException, InstantiationException, InvocationTargetException, IOException {
        final HttpRequest request =
                new HttpRequestBuilder()
                        .setPage("long.html")
                        .setConnection("keep-alive")
                        .setHost(socket.getInetAddress().getHostName() + ":" + socket.getPort())
                        .setVariable("type", factorial)
                        .setVariable("x", x.toString())
                        .build();

        return sendRequest(request);
    }

    public String checkResult(@NotNull final BigInteger id) throws IOException, IllegalAccessException, InstantiationException, InvocationTargetException {
        final HttpRequest request =
                new HttpRequestBuilder()
                        .setPage("task.html")
                        .setConnection("keep-alive")
                        .setHost(socket.getInetAddress().getHostName() + ":" + socket.getPort())
                        .setVariable("id", id.toString())
                        .build();

        return sendRequest(request);
    }

    @NotNull
    private String sendRequest(@NotNull final HttpRequest request) throws IOException {
        for (int i = 0; i < 4; i++) {
            try {
                outClient.writeBytes(request.toString());

                int countLines = 0;
                int contentLength = 0;
                while (true) {
                    final String line = inClient.readLine();

                    if (line.equals("")) {
                        countLines++;
                        if (countLines == 2) {
                            break;
                        }
                        continue;
                    }
                    countLines = 0;


                    String[] split = line.split(":");

                    if (split.length != 2) {
                        continue;
                    }

                    if (split[0].equals("Content-Length")) {
                        contentLength = Integer.parseInt(split[1].trim());
                    }
                }
                char[] data = new char[contentLength];

                int p = 0;
                while (p != data.length) {
                    p += inClient.read(data, p, data.length - p);
                }

                return new String(data);
            } catch (Exception e) {
                reconnect();
            }
        }
        return "Error: Problem with connection";
    }
}
