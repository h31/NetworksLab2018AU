package utils.response;

import http.HttpResponse;
import utils.API;

public class StopLotResponseCommand implements ResponseCommand {
    public StopLotResponseCommand(HttpResponse httpResponse) {
    }

    @Override
    public HttpResponse toHttpResponse() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.STOP_LOT;
    }
}
