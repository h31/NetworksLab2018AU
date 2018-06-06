package utils.request;

import http.HttpRequest;
import org.json.JSONObject;
import utils.API;

import java.net.URI;
import java.nio.file.Paths;

public class StopLotRequestCommand implements RequestCommand {

    private final static API api = API.STOP_LOT;

    private final int lotId;

    public StopLotRequestCommand(int lotId) {
        this.lotId = lotId;
    }

    public int getLotId() {
        return lotId;
    }

    public StopLotRequestCommand(HttpRequest httpRequest) {
        final String[] parts = httpRequest.getURI().getPath().split("/");
        this.lotId = Integer.valueOf(parts[2]);
    }

    @Override
    public HttpRequest toHttpRequest() {
        return new HttpRequest(buildUri(), api.getHttpMethod(), new JSONObject());
    }

    private URI buildUri() {
        return URI.create(Paths.get(
                "/" + api.getUriStart())
                .resolve(String.valueOf(lotId))
                .toString());
    }

    @Override
    public API getAPI() {
        return api;
    }
}
