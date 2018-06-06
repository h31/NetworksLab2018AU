package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;

public class NotImplementedResponseCommand implements ResponseCommand {
    protected final String executionResult;

    public NotImplementedResponseCommand(HttpResponse httpResponse) throws JSONException {
        JSONObject body = httpResponse.getBody();
        executionResult = body.getString(EXECUTION_RESULT);
    }

    public NotImplementedResponseCommand(String executionResult) {
        this.executionResult = "Not implemented: " + executionResult;
    }

    public NotImplementedResponseCommand() {
        this("Not implemented");
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
