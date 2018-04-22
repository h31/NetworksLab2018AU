package client;

import http.HttpRequest;
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
        final ResponseCommand responseCommand = requestCommand.getAPI().buildResponse(network.receive());
        return responseCommand.getExecutionResult();
    }

}
