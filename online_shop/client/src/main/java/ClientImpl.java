import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Map;
import java.util.Scanner;
import java.util.stream.Stream;

public class ClientImpl extends Client {

    private Socket serverSocket;
    private boolean isExit = false;

    @Override
    public void run() {
        Stream.of(commantType).forEach(System.out::println);
        Scanner sc = new Scanner(new ByteArrayInputStream(String.join("\n", textCommand).getBytes()));//System.in);
        while (!isExit) {
            String[] line = sc.nextLine().replaceAll("\\s+", " ").split(" ");
            if (line.length == 0) {
                break;
            }
            System.out.print("current command: " + String.join(" ", line));
            System.out.println();
            chooseCommand(line);
        }

    }

    @Override
    void sendGET(@NotNull String[] command) {
        if (command.length < 1) {
            return;
        }

        if (!serverSocket.isClosed()) {
            HttpRequest request = new HttpRequest("GET", "/" + Utils.TypeCommand.GET);
            System.out.println("send request: " + request);
            try {
                request.dump(serverSocket.getOutputStream());

                HttpResponse response = HttpResponse.parse(serverSocket.getInputStream());
                System.out.println("recieve response " + response);
                Map<String, Integer> shop = Utils.parseShop(response.getJSONBody());
            } catch (IOException e) {
                System.out.println(" not connect with server");
//                    e.printStackTrace();
            }
        }
    }

    @Override
    void sendHELLO(@NotNull String[] command) {
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

    @Override
    void sendADD(@NotNull String[] command) {
        if (command.length < 3) {
            return;
        }
        String prosuct = command[1];
        Integer count = Integer.valueOf(command[2]);

        if (!serverSocket.isClosed()) {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put(prosuct, count);
            HttpRequest request = new HttpRequest("GET", "/" + Utils.TypeCommand.ADD, jsonObject);
            System.out.println("send request: " + request);
            try {
                request.dump(serverSocket.getOutputStream());

                HttpResponse response = HttpResponse.parse(serverSocket.getInputStream());
                System.out.println("recieve response " + response);
                int status = response.getJSONBody().getInt("Status");
                System.out.println(" status response: " + status);
            } catch (IOException e) {
                System.out.println(" not connect with server");
//                    e.printStackTrace();
            }
        }

    }

    @Override
    void sendPAY(@NotNull String[] command) {
        if (command.length < 3) {
            return;
        }
        String prosuct = command[1];
        Integer count = Integer.valueOf(command[2]);

        if (!serverSocket.isClosed()) {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put(prosuct, count);
            HttpRequest request = new HttpRequest("GET", "/" + Utils.TypeCommand.PAY, jsonObject);
            System.out.println("send request: " + request);
            try {
                request.dump(serverSocket.getOutputStream());

                HttpResponse response = HttpResponse.parse(serverSocket.getInputStream());
                System.out.println("recieve response " + response);
                int status = response.getJSONBody().getInt("Status");
                System.out.println(" status response: " + status);
            } catch (IOException e) {
                System.out.println(" not connect with server");
//                    e.printStackTrace();
            }
        }

    }

    @Override
    void sendBYE(@NotNull String[] command) {

        if (!serverSocket.isClosed()) {
            try {

                if (!serverSocket.isClosed()) {
                    JSONObject jsonRequest = new JSONObject();
                    jsonRequest.put(Utils.JsonElement.EXIT, true);
                    HttpRequest request = new HttpRequest("GET", "/" + Utils.TypeCommand.BYE, jsonRequest);

                    System.out.println("send request: " + request);
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


                serverSocket.close();
            } catch (IOException e) {
                System.out.println(" server not closed");
//                    e.printStackTrace();
            }
        }
    }

    @Override
    void sendEXIT(@NotNull String[] command) {
        sendBYE(command);
        isExit = true;
    }

    private void chooseCommand(@NotNull String[] command) {
        switch (command[0]) {
            case Utils.TypeCommand.HELLO:
                sendHELLO(command);
                break;
            case Utils.TypeCommand.ADD:
                sendADD(command);
                break;
            case Utils.TypeCommand.GET:
                sendGET(command);
                break;
            case Utils.TypeCommand.PAY:
                sendPAY(command);
                break;
            case Utils.TypeCommand.BYE:
                sendBYE(command);
                break;
            case Utils.TypeCommand.EXIT:
                sendEXIT(command);
                break;
            default:
                break;
        }
    }

}
