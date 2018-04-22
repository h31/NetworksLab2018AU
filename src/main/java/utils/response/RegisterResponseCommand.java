package utils.response;

import http.HttpResponse;
import utils.API;

public class RegisterResponseCommand implements ResponseCommand {

    public RegisterResponseCommand(HttpResponse httpResponse) {
    }

    @Override
    public HttpResponse toHttpResponse() {
        return null;
    }

    @Override
    public API getAPI() {
        return null;
    }

}
