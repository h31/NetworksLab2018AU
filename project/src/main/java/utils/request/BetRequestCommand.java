package utils.request;

import http.HttpRequest;
import org.json.JSONObject;
import utils.API;

import java.net.URI;
import java.nio.file.Paths;

public class BetRequestCommand implements RequestCommand {

    private static final API api = API.BET;

    private final int lotId;
    private final int value;

    public int getLotId() {
        return lotId;
    }

    public int getValue() {
        return value;
    }

    public BetRequestCommand(int lotId, int value) {
        this.lotId = lotId;
        this.value = value;
    }

    public BetRequestCommand(HttpRequest httpRequest) {
        final String[] parts = httpRequest.getURI().getPath().split("/");
        this.lotId = Integer.valueOf(parts[2]);
        this.value = Integer.valueOf(parts[3]);
    }

    @Override
    public HttpRequest toHttpRequest() {
        return new HttpRequest(buildUri(), api.getHttpMethod(), new JSONObject());
    }

    private URI buildUri() {
        return URI.create(Paths.get(
                "/" + api.getUriStart())
                .resolve(String.valueOf(lotId))
                .resolve(String.valueOf(value))
                .toString());
    }

    @Override
    public API getAPI() {
        return api;
    }
}
