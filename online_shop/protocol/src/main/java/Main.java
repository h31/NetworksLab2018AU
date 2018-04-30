import org.json.JSONArray;
import org.json.JSONObject;
import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        Thread server = new Thread(() -> {
            try (
                    ServerSocket serverSocket = new ServerSocket(11111);
                    Socket socket = serverSocket.accept();
            ) {
                HttpRequest request = HttpRequest.parse(socket.getInputStream());
                String requestURL = request.getUrl();
                System.out.println(request);
                JSONObject jsonObject = new JSONObject();
                jsonObject.put("url", requestURL);
                jsonObject.put("item1_price", 666);
                jsonObject.put("item2_price", 888);
                String[] array = {"str1", "str2", "str3"};
                jsonObject.put("array_example", new JSONArray(array));
                HttpResponse response = new HttpResponse(200, "OK", jsonObject);
                response.dump(socket.getOutputStream());

            } catch (IOException e) {
                e.printStackTrace();
            }
        });

        server.start();


        String url = "localhost";
        try (Socket socket = new Socket(url, 11111)) {
            JSONObject jsonRequest = new JSONObject();
            jsonRequest.put("hello", "world");
            HttpRequest request = new HttpRequest("GET", "/superduperitemprice", jsonRequest);
            request.dump(socket.getOutputStream());
            HttpResponse response = HttpResponse.parse(socket.getInputStream());
            System.out.println(response);
            List<Object> responseArrayExtracted = response.getJSONBody().getJSONArray("array_example").toList();
            for (Object o : responseArrayExtracted) {
                System.out.println(o);
            }
        }

    }
}