package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;

import static http.HttpResponse.STATUS_OK;

public class BetResponseCommand implements ResponseCommand {
    private final static String LOT_ID = "lotId";
    private final static String ERROR_DESCRIPTION = "executionResult";
    private final int lotId;
    private final String executionResult;


    public BetResponseCommand(HttpResponse httpResponse) {
       JSONObject body = httpResponse.getBody();
       try {
           lotId = body.getInt(LOT_ID);
           executionResult = body.getString(ERROR_DESCRIPTION);
       } catch (JSONException e) {
           throw new IllegalStateException("No such field: lotId/executionResult");
       }
    }

    public BetResponseCommand(int lotId, String executionResult) {
        this.lotId = lotId;
        this.executionResult = executionResult;
    }

    @Override
    public HttpResponse toHttpResponse() {
        try {
            JSONObject body = new JSONObject().put(LOT_ID, lotId).put(ERROR_DESCRIPTION, executionResult);
            return new HttpResponse(STATUS_OK, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    @Override
    public API getAPI() {
        return API.BET;
    }

    public int getLotId() {
        return lotId;
    }

    public String getExecutionResult() {
        return executionResult;
    }
}
