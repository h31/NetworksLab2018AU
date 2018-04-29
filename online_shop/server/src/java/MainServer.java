package src.java;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class MainServer {

    public static void main(String[] args) {
        while (true) {
            try (ServerSocket serverSocket = new ServerSocket(
                    Utils.PORT_SERVER);
            ) {
                Socket clientSocket = serverSocket.accept();
                try (BufferedWriter out =
                             new BufferedWriter(
                                     new OutputStreamWriter(clientSocket.getOutputStream()));
                     BufferedReader in = new BufferedReader(
                             new InputStreamReader(clientSocket.getInputStream()))) {
                    in.lines().forEach(System.out::println);
                    out.write("hello Boris");
                    out.flush();
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
