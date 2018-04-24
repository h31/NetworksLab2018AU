package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import static http.HttpResponse.STATUS_NOT_FOUND;
import static http.HttpResponse.STATUS_OK;

public class NotFoundResponseCommand implements ResponseCommand {
    private final static String NOT_FOUND = "NOT FOUND";
    private final static NotFoundResponseCommand INSTANCE;
    private final static HttpResponse RESPONSE_INSTANCE;

    static {
        INSTANCE = new NotFoundResponseCommand();
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, NOT_FOUND);
            RESPONSE_INSTANCE = new HttpResponse(STATUS_NOT_FOUND, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    private NotFoundResponseCommand() {}

    public static ResponseCommand getInstance() {
        return INSTANCE;
    }

    @Override
    public HttpResponse toHttpResponse() {
        return RESPONSE_INSTANCE;
    }

    @Override
    public API getAPI() {
        return API.NOT_FOUND;
    }

    @Override
    public String getExecutionResult() {
        return NOT_FOUND;
    }
}
