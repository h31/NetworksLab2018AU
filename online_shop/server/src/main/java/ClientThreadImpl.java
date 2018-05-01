import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.IOException;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;

public class ClientThreadImpl extends ClientThread {

    private String nameClient;
    private Socket clientSocket;
    private boolean isActive = true;


    public ClientThreadImpl(@NotNull Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    @Override
    protected void handlerHELLO(@NotNull HttpRequest request) throws IOException {
        System.out.println(" handlerHELLO");

        nameClient = request.getJSONBody().getString(Utils.JsonElement.NAME);
        System.out.println("accept client with name " + nameClient);

        JSONObject jsonObject = initJson(request.getUrl());
        jsonObject.put(Utils.JsonElement.STATUS, nameClient == null ? Utils.Status.EMPTY_NAME : Utils.Status.OK);

        HttpResponse response = new HttpResponse(200, "OK", jsonObject);
        System.out.println("response :" + response);

        response.dump(clientSocket.getOutputStream());

        if (nameClient != null) {
            MainServer.clients.put(nameClient, clientSocket);
        }
    }

    @Override
    protected void handlerGET(@NotNull HttpRequest request) throws IOException {
        System.out.println(" handlerGET");
        JSONObject jsonObject = initJson(request.getUrl());
        Utils.dumpShop(MainServer.shop.getShop(), jsonObject);

        HttpResponse response = new HttpResponse(200, "OK", jsonObject);
        System.out.println("response :" + response);

        response.dump(clientSocket.getOutputStream());
    }

    @Override
    protected void handlerADD(@NotNull HttpRequest request) throws IOException {
        System.out.println(" handlerADD");

        MainServer.shop.addProductsToShop(parseProducts(request.getJSONBody()));

        JSONObject jsonObject = initJson(request.getUrl());
        jsonObject.put(Utils.JsonElement.STATUS, Utils.Status.OK);

        HttpResponse response = new HttpResponse(200, "OK", jsonObject);
        System.out.println("response :" + response);
        response.dump(clientSocket.getOutputStream());

    }

    @Override
    protected void handlerPAY(@NotNull HttpRequest request) throws IOException {
        System.out.println(" handlerPAY");

        boolean result = MainServer.shop.payProductsFromShop(parseProducts(request.getJSONBody()));

        JSONObject jsonObject = initJson(request.getUrl());
        jsonObject.put(Utils.JsonElement.STATUS, result ? Utils.Status.OK : Utils.Status.FAIL);

        HttpResponse response = new HttpResponse(200, "OK", jsonObject);
        System.out.println("response :" + response);
        response.dump(clientSocket.getOutputStream());
    }

    @Override
    public boolean handlerBYE(@NotNull HttpRequest request) throws IOException {
        if (request.getJSONBody().getBoolean(Utils.JsonElement.EXIT)) {
            JSONObject jsonObject = initJson(request.getUrl());
            jsonObject.put(Utils.JsonElement.STATUS, nameClient == null ? Utils.Status.EMPTY_NAME : Utils.Status.OK);

            HttpResponse response = new HttpResponse(200, "OK", jsonObject);
            response.dump(clientSocket.getOutputStream());

            MainServer.clients.remove(nameClient);
            clientSocket.close();
            System.out.println("Client with name " + nameClient + " exit");
            return true;
        }
        return false;
    }

    @Override
    public void run() {
        while (isActive && !clientSocket.isClosed()) {
            try {
                HttpRequest request = HttpRequest.parse(clientSocket.getInputStream());
                System.out.println(" get request " + request);
                chooseCommand(request);
            } catch (IOException e) {
                System.out.println("not connect with client");
//                e.printStackTrace();
            }
        }
    }

    private void chooseCommand(@NotNull HttpRequest request) throws IOException {
        switch (request.getUrl().substring(1)) {
            case Utils.TypeCommand.HELLO:
                handlerHELLO(request);
                break;
            case Utils.TypeCommand.EXIT:
            case Utils.TypeCommand.BYE:
                if (handlerBYE(request)) {
                    return;
                }
                break;
            case Utils.TypeCommand.GET:
                handlerGET(request);
                break;
            case Utils.TypeCommand.ADD:
                handlerADD(request);
                break;
            case Utils.TypeCommand.PAY:
                handlerPAY(request);
                break;
            default:
                System.out.println("command unknown");
                break;

        }

    }

    private @NotNull JSONObject initJson(String url) {
        JSONObject jsonObject = new JSONObject();
        jsonObject.put(Utils.JsonElement.URL, url);
        return jsonObject;
    }

    private @NotNull Map<String, Integer> parseProducts(@NotNull JSONObject jsonObject) {
        Map<String, Integer> products = new HashMap<>();
        jsonObject.keySet().forEach(item -> products.put(item.toLowerCase(), jsonObject.getInt(item)));
        return products;
    }
}
