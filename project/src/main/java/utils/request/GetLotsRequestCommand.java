package utils.request;

import http.HttpRequest;
import org.json.JSONObject;
import utils.API;

import java.net.URI;
import java.nio.file.Paths;

public class GetLotsRequestCommand implements RequestCommand {

    private static final API api = API.GET_LOTS;
    private static final URI REQUEST_URI = URI.create(Paths.get("/" + api.getUriStart()).toString());

    public GetLotsRequestCommand() {

    }

    public GetLotsRequestCommand(HttpRequest httpRequest) {

    }

    @Override
    public HttpRequest toHttpRequest() {
        return new HttpRequest(REQUEST_URI, api.getHttpMethod(), new JSONObject());
    }

    @Override
    public API getAPI() {
        return api;
    }
}
