import java.io.IOException;
import java.net.Socket;

public class Main {
    public static void main(String[] args) throws IOException {
        String url = "google.co.uk";
        try (Socket socket = new Socket(url, 80)) {
            socket.setSoTimeout(1000);
            HttpRequest request = new HttpRequest("GET", "/intl/en/policies/privacy/");
            System.out.println(request);
            request.dump(socket.getOutputStream());
            HttpResponse response = HttpResponse.parse(socket.getInputStream());
            System.out.println(response);
        }

    }
}