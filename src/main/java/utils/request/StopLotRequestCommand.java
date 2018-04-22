package utils.request;

import http.HttpRequest;
import utils.API;

public class StopLotRequestCommand implements RequestCommand {
    public StopLotRequestCommand(HttpRequest httpRequest) {
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
