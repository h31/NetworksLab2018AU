package client;

import http.HttpRequest;
import http.HttpResponse;
import org.json.JSONException;
import server.StatusCode;
import utils.UnknownStatusCodeException;
import utils.request.RequestCommand;
import utils.response.ResponseCommand;

import java.io.IOException;

public class Logic {

    private final Network network;

    public Logic(Network network) {
        this.network = network;
    }

    public String process(RequestCommand requestCommand) throws JSONException, UnknownStatusCodeException, IOException {
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
