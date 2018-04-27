package client;

import http.HttpRequest;
import http.HttpResponse;
import org.json.JSONException;
import utils.UnknownStatusCodeException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class Network {

    private final String host;
    private final int port;

    private Socket socket;
    private PrintWriter writer;
    private BufferedReader reader;

    public Network(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public void start() throws IOException {
        this.socket = new Socket(host, port);
        this.writer = new PrintWriter(socket.getOutputStream());
        this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    public void send(HttpRequest httpRequest) {
        for (String line : httpRequest.toStrings()) {
            writer.println(line);
        }
        writer.flush();
    }

    public HttpResponse receive() throws UnknownStatusCodeException, JSONException {
        final List<String> lines = new ArrayList<>();
        try {
            boolean started = false;
            while (!started) {
                while (reader.ready()) {
                    started = true;
                    lines.add(reader.readLine());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new HttpResponse(lines);
    }

    public void terminate() throws IOException {
        writer.close();
        reader.close();
        socket.close();
    }

}
