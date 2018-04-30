import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainServer {

    private Map<String, Socket> clients = new HashMap<>();


    class ClientThread implements Runnable {

        private String nameClient;
        private boolean isActive = true;

        public ClientThread(String nameClient) {
            this.nameClient = nameClient;
        }

        public String getNameClient() {
            return nameClient;
        }

        public boolean isActive() {
            return isActive;
        }

        @Override
        public void run() {
            Socket clientSocket = clients.get(nameClient);
            while (isActive && !clientSocket.isClosed()) {
                try {
                    HttpRequest request = HttpRequest.parse(clientSocket.getInputStream());
                    String requestURL = request.getUrl();
                    List<String> responseBody = Arrays.asList(requestURL, " HelloBoris");
                    HttpResponse response = new HttpResponse(200, "OK",
                            responseBody);
                    response.dump(clientSocket.getOutputStream());
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void runServer() {
        while (true) {
            try (ServerSocket serverSocket = new ServerSocket(Utils.PORT_SERVER)) {
                Socket clientSocket = serverSocket.accept();

                ClientThread client = new ClientThread("name");
                clients.put(client.getNameClient(), clientSocket);
                new Thread(client).start();


            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }


    public static void main(String[] args) {
        MainServer server = new MainServer();
        Thread main_server = new Thread(server::runServer);
        main_server.start();

    }
}
