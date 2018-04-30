import org.json.JSONObject;
import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;
import java.util.stream.Collector;
import java.util.stream.Stream;


public class MainClient {

    static final String[] commantType = {
            "Hello in Online Shop",
            "Command list: ",
            " ",
            "HELLO name host port    - start connect with shop serer",
            "GET                     - get all shop product",
            "ADD name_product count  - add product in shop",
            "PAY name_product count  - pay product from shop",
            "BYE                     - stop connection with shop server",
            "EXIT                    - stop application",
            ""
    };


    static final String[] textCommand = {
            "HELLO kate localhost 8000",
            " GET  ",
            " ADD apple 100",
            "GET",
            "PAY apple 1",
            "GET ",
            "ADD coca 10",
            "GET ",
            "PAY apple 10",
            "GET ",
            "PAY coca 10",
            "GET ",
            "BYE",
            "EXIT"

    };


    public static class Client implements Runnable {

        private Socket serverSocket;
        private boolean isExit = false;

        @Override
        public void run() {
            Stream.of(commantType).forEach(System.out::println);

            while (!isExit) {
                Scanner sc = new Scanner(new ByteArrayInputStream(String.join("\n", textCommand).getBytes()));//System.in);
                String[] line = sc.nextLine().replaceAll("\\s+", " ").split(" ");
                if (line.length == 0) {
                    break;
                }
                System.out.print("current command: " + String.join(" ", line));
                System.out.println();
                switch (line[0]) {
                    case Utils.TypeCommand.HELLO:
                        sendHELLO(line);
                        break;
                    case Utils.TypeCommand.ADD:
                        sendADD(line);
                        break;
                    case Utils.TypeCommand.GET:
                        sendGET(line);
                        break;
                    case Utils.TypeCommand.PAY:
                        sendPAY(line);
                        break;
                    case Utils.TypeCommand.BYE:
                        sendBYE(line);
                        break;
                    case Utils.TypeCommand.EXIT:
                        sendEXIT(line);
                        break;
                    default:
                        break;
                }
            }

        }


        public void sendGET(String[] command) {

        }

        public void sendHELLO(String[] command) {
            if (command.length < 4) {
                System.out.println(" command HELLO not correct ");
                return;
            }
            String nameClient = command[1];
            String url = command[2];
            Integer port = Integer.valueOf(command[3]);
            try {
                serverSocket = new Socket(url, port);
            } catch (IOException e) {
                System.out.println(" connect is not created");
//                e.printStackTrace();
            }

            if (!serverSocket.isClosed()) {
                JSONObject jsonRequest = new JSONObject();
                jsonRequest.put(Utils.JsonElement.NAME, nameClient);
                HttpRequest request = new HttpRequest("GET", "/" + Utils.TypeCommand.HELLO, jsonRequest);

                System.out.println("send request: " + request);
                System.out.println("Name: " + request.getJSONBody());
                System.out.println("Name: " + request.getJSONBody().getString(Utils.JsonElement.NAME));
                try {
                    request.dump(serverSocket.getOutputStream());

                    HttpResponse response = HttpResponse.parse(serverSocket.getInputStream());
                    System.out.println("recieve response " + response);
                    int status = response.getJSONBody().getInt("Status");
                    System.out.println(" status response: " + status);

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }


        }

        public void sendADD(String[] command) {

        }

        public void sendPAY(String[] command) {

        }

        public void sendBYE(String[] command) {

            if (!serverSocket.isClosed()) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    System.out.println(" server not closed");
//                    e.printStackTrace();
                }
            }
        }

        public void sendEXIT(String[] command) {
            sendBYE(command);
            isExit = true;
        }

    }

    public static void main(String[] args) {
        Client client = new Client();
        new Thread(client).start();
    }

}
