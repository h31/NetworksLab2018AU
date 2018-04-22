package utils.response;

import http.HttpResponse;
import utils.API;

public class BetResponseCommand implements ResponseCommand {

    public BetResponseCommand(HttpResponse httpResponse) {
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
