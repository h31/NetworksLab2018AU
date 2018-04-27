package server;

import utils.data.Lot;
import utils.data.User;
import utils.data.UserRole;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class Main {
    private static volatile boolean running = true;

    public static void main(String[] args) throws IOException {
        Map<Integer, Lot> lots = new ConcurrentHashMap<>();
        Set<User> users = ConcurrentHashMap.newKeySet();
        ServerSocket serverSocket = new ServerSocket(Integer.valueOf(args[0]));
        List<Thread> clientThreads = new ArrayList<>();
        List<Logic> logics = new ArrayList<>();
        Thread consoleReaderThread = new Thread(Main::read);
        consoleReaderThread.start();
        while (running) {
            Socket clientSocket = serverSocket.accept();
            Context context = new Context(lots, users);
            Logic logic = new Logic(context, clientSocket);
                logics.add(logic);
            Thread thread = new Thread(logic::start);
            clientThreads.add(thread);
            thread.start();
        }
        for (int i = 0; i < clientThreads.size(); i++) {
            Logic logic = logics.get(i);
            Thread clientThread = clientThreads.get(i);
            logic.stop();
            clientThread.interrupt();
        }
    }

    private static void read() {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        try {
            while (!bufferedReader.readLine().equals("stop")) {
                System.out.println(">>");
            }
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
        running = false;
    }
}
