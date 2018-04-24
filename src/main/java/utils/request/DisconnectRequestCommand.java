package utils.request;

import http.HttpRequest;
import utils.API;

//TODO implement
public class DisconnectRequestCommand implements RequestCommand {
    public DisconnectRequestCommand(HttpRequest httpRequest) {
    }

    @Override
    public HttpRequest toHttpRequest() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.DISCONNECT;
    }
}
