package utils.response;

import http.HttpResponse;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import utils.API;
import utils.data.Lot;
import utils.request.GetLotsRequestCommand;

import java.util.ArrayList;
import java.util.List;

import static http.HttpResponse.STATUS_OK;

public class GetLotsResponseCommand implements ResponseCommand {
    private static final String LOTS = "LOTS";
    private final String executionResult;
    private final List<Lot> lots;

    public GetLotsResponseCommand(HttpResponse httpResponse) {
        JSONObject body = httpResponse.getBody();
        lots = new ArrayList<>();
        try {
            JSONArray jsonArray = body.getJSONArray(LOTS);
            for (int i = 0; i < jsonArray.length(); i++) {
                JSONObject jsonObject = jsonArray.getJSONObject(i);
                Lot lot = new Lot(jsonObject);
                lots.add(lot);
            }
            executionResult = body.getString(EXECUTION_RESULT);
        } catch (JSONException e) {
            throw new IllegalStateException("No such field: executionResult/lots");
        }
    }

    public GetLotsResponseCommand(String executionResult, List<Lot> lots) {
        this.executionResult = executionResult;
        this.lots = lots;
    }

    @Override
    public HttpResponse toHttpResponse() {
        try {
            JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
            JSONArray jsonArray = new JSONArray();
            for (Lot lot: lots) {
                //TODO ???
                jsonArray.put(lot.toJSONObject());
            }
            return new HttpResponse(STATUS_OK, body);
        } catch(JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    @Override
    public API getAPI() {
        return API.GET_LOTS;
    }

    @Override
    public String getExecutionResult() {
        return executionResult;
    }
}
