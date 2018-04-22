package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
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
        return null;
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
