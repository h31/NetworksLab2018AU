package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;

public class ForbiddenResponseCommand implements ResponseCommand {
    protected final String executionResult;

    public ForbiddenResponseCommand(HttpResponse httpResponse) throws JSONException {
        JSONObject body = httpResponse.getBody();
        executionResult = body.getString(EXECUTION_RESULT);
    }

    public ForbiddenResponseCommand(String executionResult) {
        this.executionResult = executionResult;
    }

    @Override
    public HttpResponse toHttpResponse() throws JSONException {
        JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
        return new HttpResponse(StatusCode.NOT_IMPLEMENTED, body);
    }

    @Override
    public API getAPI() {
        return null;
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }
}
