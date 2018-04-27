package server;

import http.HttpRequest;
import http.HttpResponse;
import org.json.JSONException;

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

    public void start() throws IOException {
        this.writer = new PrintWriter(socket.getOutputStream());
        this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    public void send(HttpResponse httpResponse) {
        for (String line : httpResponse.toStrings()) {
            writer.println(line);
        }
        writer.flush();
    }

    public HttpRequest receive() throws IOException, JSONException {
        final List<String> lines = new ArrayList<>();
        boolean started = false;
        while (!started) {
            while (reader.ready()) {
                started = true;
                lines.add(reader.readLine());
            }
        }
        return new HttpRequest(lines);
    }

    public void terminate() throws IOException {
        writer.close();
        reader.close();
        socket.close();
    }
}

