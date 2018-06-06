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

    public static void main(String[] args) {
        Map<Integer, Lot> lots = new ConcurrentHashMap<>();
        Set<User> users = ConcurrentHashMap.newKeySet();
        ServerSocket serverSocket;
        try {
            serverSocket = new ServerSocket(Integer.valueOf(args[0]));
        } catch (IOException e) {
            System.out.println(e.getMessage());
            return;
        }
        Set<Socket> clientSockets = ConcurrentHashMap.newKeySet();
        Thread consoleReaderThread = new Thread(Main::read);
        consoleReaderThread.start();
        while (running) {
            Socket clientSocket;
            try {
                clientSocket = serverSocket.accept();
            } catch (IOException e) {
                System.out.println(e.getMessage());
                stop();
                return;
            }
            Context context = new Context(lots, users, clientSocket, clientSockets);
            Logic logic;
            try {
                logic = new Logic(context, clientSocket);
            } catch (IOException e) {
                System.out.println(e.getMessage());
                continue;
            }
            clientSockets.add(clientSocket);
            Thread thread = new Thread(logic::start);
            thread.start();
        }
        try {
            serverSocket.close();
        } catch (IOException ignored) {}
    }

    private static void read() {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        try {
            do {
                System.out.println(">>");
            } while (!bufferedReader.readLine().equals("stop") && running);
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
        stop();

    }

    private static void stop() {
        running = false;
    }
}
