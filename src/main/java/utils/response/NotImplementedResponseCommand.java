package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;

public class NotImplementedResponseCommand implements ResponseCommand {
    protected final String executionResult;

    public NotImplementedResponseCommand(HttpResponse httpResponse) {
        JSONObject body = httpResponse.getBody();
        try {
            executionResult = body.getString(EXECUTION_RESULT);
        } catch (JSONException e) {
            throw new IllegalStateException("No such field: executionResult");
        }
    }

    public NotImplementedResponseCommand(String executionResult) {
        this.executionResult = executionResult;
    }

    @Override
    public HttpResponse toHttpResponse() {
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
            return new HttpResponse(StatusCode.NOT_IMPLEMENTED, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
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
