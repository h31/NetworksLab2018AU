package utils.response;

import http.HttpResponse;
import utils.API;

public class NewLotResponseCommand implements ResponseCommand {
    private final String executionResult;

    public NewLotResponseCommand(HttpResponse httpResponse) {
    }

    @Override
    public HttpResponse toHttpResponse() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.NEW_LOT;
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }
}
