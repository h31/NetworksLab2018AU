package utils.response;

import http.HttpResponse;
import utils.API;

public class DisconnectResponseCommand implements ResponseCommand {
    @Override
    public HttpResponse toHttpResponse() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.DISCONNECT;
    }

    @Override
    public String getExecutionResult() {
        return null;
    }
}
