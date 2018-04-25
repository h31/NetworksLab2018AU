package client;

import http.HttpRequest;
import http.HttpResponse;
import server.StatusCode;
import utils.request.RequestCommand;
import utils.response.ResponseCommand;

public class Logic {

    private final Network network;

    public Logic(Network network) {
        this.network = network;
    }

    public String process(RequestCommand requestCommand) {
        final HttpRequest httpRequest = requestCommand.toHttpRequest();
        network.send(httpRequest);
        final HttpResponse response = network.receive();
        ResponseCommand responseCommand;
        if (response.getStatusCode() == StatusCode.OK) {
            responseCommand = requestCommand.getAPI().buildResponse(response);
        } else {
            responseCommand = response.getStatusCode().buildResponse(response);
        }
        return responseCommand.getExecutionResult();
    }

}
