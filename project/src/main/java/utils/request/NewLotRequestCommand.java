package utils.request;

import http.HttpRequest;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import java.net.URI;
import java.nio.file.Paths;

public class NewLotRequestCommand implements RequestCommand {

    private final static API api = API.NEW_LOT;
    private final static URI REQUEST_URI = URI.create(Paths.get("/" + api.getUriStart()).toString());
    private final static String DESC_FIELD = "desc";
    private final static String START_VALUE_FIELD = "startValue";

    private final String description;
    private final int startValue;

    public NewLotRequestCommand(String description, int startValue) {
        this.description = description;
        this.startValue = startValue;
    }

    public String getDescription() {
        return description;
    }

    public int getStartValue() {
        return startValue;
    }

    public NewLotRequestCommand(HttpRequest httpRequest) throws JSONException {
        this.description = httpRequest.getBody().getString(DESC_FIELD);
        this.startValue = httpRequest.getBody().getInt(START_VALUE_FIELD);
    }

    @Override
    public HttpRequest toHttpRequest() throws JSONException {
        return new HttpRequest(REQUEST_URI, api.getHttpMethod(),
                new JSONObject()
                        .put(DESC_FIELD, description)
                        .put(START_VALUE_FIELD, startValue)
            );
    }

    @Override
    public API getAPI() {
        return API.NEW_LOT;
    }
}
