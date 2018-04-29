package src.java;

import src.main.java.HttpRequest;
import src.main.java.HttpResponse;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;

public class MainServer {

    public static void main(String[] args) {
        while (true) {
            try (ServerSocket serverSocket = new ServerSocket(
                    Utils.PORT_SERVER);
            ) {
                Socket clientSocket = serverSocket.accept();
                HttpRequest request = HttpRequest.parse(clientSocket.getInputStream());
                String requestURL = request.getUrl();
                List<String> responseBody = Arrays.asList(requestURL, " HelloBoris");
                HttpResponse response = new HttpResponse(200, "OK", responseBody);
                response.dump(clientSocket.getOutputStream());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
