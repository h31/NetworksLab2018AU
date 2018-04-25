package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import static http.HttpResponse.STATUS_FORBIDDEN;

public class ForbiddenResponseCommand implements ResponseCommand {
    private final static String FORBIDDEN = "FORBIDDEN";
    private final static ForbiddenResponseCommand INSTANCE;
    private final static HttpResponse RESPONSE_INSTANCE;

    static {
        INSTANCE = new ForbiddenResponseCommand();
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, FORBIDDEN);
            RESPONSE_INSTANCE = new HttpResponse(STATUS_FORBIDDEN, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    private ForbiddenResponseCommand() {}

    public static ForbiddenResponseCommand getInstance() {
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
        return FORBIDDEN;
    }
}
