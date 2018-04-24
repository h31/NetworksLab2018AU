package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import static http.HttpResponse.STATUS_NOT_IMPLEMENTED;

public class NotImplementedResponseCommand implements ResponseCommand {
    private final static String NOT_IMPLEMENTED = "NOT IMPLEMENTED";
    private final static NotImplementedResponseCommand INSTANCE;
    private final static HttpResponse RESPONSE_INSTANCE;

    static {
        INSTANCE = new NotImplementedResponseCommand();
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, NOT_IMPLEMENTED);
            RESPONSE_INSTANCE = new HttpResponse(STATUS_NOT_IMPLEMENTED, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    private NotImplementedResponseCommand() {}

    public static ResponseCommand getInstance() {
        return INSTANCE;
    }

    @Override
    public HttpResponse toHttpResponse() {
        return RESPONSE_INSTANCE;
    }

    @Override
    public API getAPI() {
        return null;
    }

    @Override
    public String getExecutionResult() {
        return NOT_IMPLEMENTED;
    }
}
