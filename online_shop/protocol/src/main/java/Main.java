import src.java.Utils;
import src.main.java.HttpRequest;
import src.main.java.HttpResponse;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
//        Thread server = new Thread(() -> {
//            try (
//                    ServerSocket serverSocket = new ServerSocket(Utils.PORT_SERVER);
//                    Socket socket = serverSocket.accept();
//            ) {
//
//                HttpRequest request = HttpRequest.parse(socket.getInputStream());
//                String requestURL = request.getUrl();
//                List<String> responseBody = Arrays.asList(requestURL, "666");
//                HttpResponse response = new HttpResponse(200, "OK", responseBody);
//                response.dump(socket.getOutputStream());
//
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
//        });
//
//        server.start();


        try (Socket socket = new Socket(Utils.ADDRESS_SERVER, Utils.PORT_SERVER)) {
            socket.setSoTimeout(Utils.SEND_TIMEOUT);
            HttpRequest request = new HttpRequest("GET", "/superduperitemprice");
            System.out.println(request);
            request.dump(socket.getOutputStream());
            HttpResponse response = HttpResponse.parse(socket.getInputStream());
            System.out.println(response);
        }

    }
}