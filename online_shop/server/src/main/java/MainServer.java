import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class MainServer {

    public static final ConcurrentMap<String, Socket> clients = new ConcurrentHashMap<>();
    public static final OnlineShop shop = new OnlineShop();


    public static void runServer() {
        while (true) {
            try (ServerSocket serverSocket = new ServerSocket(Utils.PORT_SERVER)) {
                Socket clientSocket = serverSocket.accept();
                ClientThread client = new ClientThreadImpl(clientSocket);
                new Thread(client).start();


            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }
    }

    public static void main(String[] args) {
        Thread main_server = new Thread(MainServer::runServer);
        main_server.start();
    }
}
