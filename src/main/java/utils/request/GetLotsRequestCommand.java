package utils.request;

import http.HttpRequest;
import utils.API;

public class GetLotsRequestCommand implements RequestCommand {
    public GetLotsRequestCommand(HttpRequest httpRequest) {
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
