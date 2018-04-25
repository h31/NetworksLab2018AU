package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;

public class NewLotResponseCommand implements ResponseCommand {
    private final String executionResult;
    private final static String LOT_ID = "lotId";
    private final int lotId;

    public NewLotResponseCommand(HttpResponse httpResponse) {
        JSONObject body = httpResponse.getBody();
        try {
            executionResult = body.getString(EXECUTION_RESULT);
            lotId = body.getInt(LOT_ID);
        } catch (JSONException e) {
            throw new IllegalStateException("No such field executionResult/lotId");
        }
    }

    public NewLotResponseCommand(String executionResult, int lotId) {
        this.executionResult = executionResult;
        this.lotId = lotId;
    }

    @Override
    public HttpResponse toHttpResponse() {
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult).put(LOT_ID, lotId);
            return new HttpResponse(StatusCode.OK, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    @Override
    public API getAPI() {
        return API.NEW_LOT;
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }

    public int getLotId() {
        return lotId;
    }
}
