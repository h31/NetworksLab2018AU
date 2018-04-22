package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import static http.HttpResponse.STATUS_OK;

public class RegisterResponseCommand implements ResponseCommand {
    private final String executionResult;

    public RegisterResponseCommand(HttpResponse httpResponse) {
        JSONObject body = httpResponse.getBody();
        try {
            executionResult = body.getString(EXECUTION_RESULT);
        } catch (JSONException e) {
            throw new IllegalStateException("No such field: executionResult");
        }
    }

    public RegisterResponseCommand(String executionResult) {
        this.executionResult = executionResult;
    }

    @Override
    public HttpResponse toHttpResponse() {
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
            return new HttpResponse(STATUS_OK, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    @Override
    public API getAPI() {
        return API.REGISTER;
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }
}
