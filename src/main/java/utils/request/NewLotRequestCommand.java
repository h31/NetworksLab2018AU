package utils.request;

import http.HttpRequest;
import utils.API;

public class NewLotRequestCommand implements RequestCommand {
    public NewLotRequestCommand(HttpRequest httpRequest) {
    }

    @Override
    public HttpRequest toHttpRequest() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.NEW_LOT;
    }
}
