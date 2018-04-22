package utils.request;

import http.HttpRequest;
import utils.API;

public class BetRequestCommand implements RequestCommand {
    public BetRequestCommand(HttpRequest httpRequest) {
    }

    @Override
    public HttpRequest toHttpRequest() {
        return null;
    }

    @Override
    public API getAPI() {
        return null;
    }
}
