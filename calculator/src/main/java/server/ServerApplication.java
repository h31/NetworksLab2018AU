package server;

public class ServerApplication extends Thread {

    public static void main(String args[]) throws Exception {
        int longTaskLimit = Integer.MAX_VALUE;
        if (args.length == 2) {
            longTaskLimit = Integer.parseInt(args[1]);
        }
        Server server = new Server((short) 5000, longTaskLimit);
        server.start();
        System.out.println("Server started on port 5000");
        server.join();
    }
}