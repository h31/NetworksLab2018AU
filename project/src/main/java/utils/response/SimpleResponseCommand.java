package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;

public abstract class SimpleResponseCommand implements ResponseCommand {
    protected final String executionResult;

    SimpleResponseCommand(HttpResponse httpResponse) throws JSONException {
        JSONObject body = httpResponse.getBody();
        executionResult = body.getString(EXECUTION_RESULT);
    }

    SimpleResponseCommand(String executionResult) {
        this.executionResult = executionResult;
    }

    @Override
    public HttpResponse toHttpResponse() throws JSONException {
        JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
        return new HttpResponse(StatusCode.OK, body);
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }
}
