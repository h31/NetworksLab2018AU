import org.json.JSONArray;
import org.json.JSONObject;
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

        Socket clientSocket;
        private boolean isActive = true;


        public ClientThread(Socket clientSocket) {
            this.clientSocket = clientSocket;
        }

//        public String getNameClient() {
//            return nameClient;
//        }
//
//        public boolean isActive() {
//            return isActive;
//        }

        @Override
        public void run() {
            while (isActive && !clientSocket.isClosed()) {
                try {
                    HttpRequest request = HttpRequest.parse(clientSocket.getInputStream());
                    String requestURL = request.getUrl();


                    System.out.println(request);
                    System.out.println(request.getBody());
                    System.out.println(request.getJSONBody().getString(Utils.JsonElement.NAME));
                    nameClient = request.getJSONBody().getString(Utils.JsonElement.NAME);

                    JSONObject jsonObject = new JSONObject();
                    jsonObject.put("url", requestURL);
                    jsonObject.put("Status", nameClient == null ? Utils.Status.EMPTY_NAME : Utils.Status.OK);

                    HttpResponse response = new HttpResponse(200, "OK", jsonObject);
                    response.dump(clientSocket.getOutputStream());

                    if (nameClient != null) {
                        clients.put(nameClient, clientSocket);
                    }

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
                ClientThread client = new ClientThread(clientSocket);
                new Thread(client).start();


            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }

    }


    public static void main(String[] args) {
        MainServer server = new MainServer();
        Thread main_server = new Thread(server::runServer);
        main_server.start();

    }
}
