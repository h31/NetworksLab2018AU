package utils.request;

import http.HttpRequest;
import utils.API;

public class RegisterRequestCommand implements RequestCommand {
    public RegisterRequestCommand(HttpRequest httpRequest) {
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
