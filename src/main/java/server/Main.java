package server;

import utils.data.Lot;
import utils.data.User;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class Main {
    private static volatile boolean running = true;

    public static void main(String[] args) throws IOException {
        Map<Integer, Lot> lots = new ConcurrentHashMap<>();
        Set<User> users = ConcurrentHashMap.newKeySet();
        ServerSocket serverSocket = new ServerSocket(Integer.valueOf(args[0]));
        Set<Socket> clientSockets = ConcurrentHashMap.newKeySet();
        Thread consoleReaderThread = new Thread(Main::read);
        consoleReaderThread.start();
        while (running) {
            Socket clientSocket = serverSocket.accept();
            clientSockets.add(clientSocket);
            Context context = new Context(lots, users, clientSocket, clientSockets);
            Logic logic = new Logic(context, clientSocket);
            Thread thread = new Thread(logic::start);
            thread.start();
        }
        serverSocket.close();
    }

    private static void read() {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        try {
            do {
                System.out.println(">>");
            } while (!bufferedReader.readLine().equals("stop"));
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
        running = false;
    }
}
