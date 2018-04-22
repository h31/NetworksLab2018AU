package utils.response;

import http.HttpResponse;
import utils.API;

public class NewLotResponseCommand implements ResponseCommand {
    public NewLotResponseCommand(HttpResponse httpResponse) {
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
