package server;

import http.HttpRequest;
import http.HttpResponse;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class Network {
    private final Socket socket;
    private PrintWriter writer;
    private BufferedReader reader;

    public Network(Socket socket) {
        this.socket = socket;
    }

    public void start() {
        try {
            this.writer = new PrintWriter(socket.getOutputStream());
            this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (IOException e) {
            throw new IllegalStateException("can't сreate reader/writer", e);
        }
    }

    public void send(HttpResponse httpResponse) {
        for (String line : httpResponse.toStrings()) {
            writer.println(line);
        }
        writer.flush();
    }

    public HttpRequest receive() {
        final List<String> lines = new ArrayList<>();
        try {
            while (reader.ready()) {
                lines.add(reader.readLine());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new HttpRequest(lines);
    }

    public void terminate() {
        writer.close();
        try {
            reader.close();
            socket.close();
        } catch (IOException e) {
            throw new IllegalStateException(e);
        }
    }
}

