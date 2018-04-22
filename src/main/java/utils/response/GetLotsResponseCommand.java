package utils.response;

import http.HttpResponse;
import utils.API;

public class GetLotsResponseCommand implements ResponseCommand {
    public GetLotsResponseCommand(HttpResponse httpResponse) {
    }

    @Override
    public HttpResponse toHttpResponse() {
        return null;
    }

    @Override
    public API getAPI() {
        return API.GET_LOTS;
    }
}
