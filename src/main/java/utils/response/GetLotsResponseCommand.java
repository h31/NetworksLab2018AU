package utils.response;

import http.HttpResponse;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;
import utils.data.Lot;
import utils.request.GetLotsRequestCommand;

import java.util.ArrayList;
import java.util.List;


public class GetLotsResponseCommand implements ResponseCommand {
    private static final String LOTS = "LOTS";
    private final String executionResult;
    private final List<Lot> lots;

    public GetLotsResponseCommand(HttpResponse httpResponse) {
        JSONObject body = httpResponse.getBody();
        System.out.println(body);
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
            throw new IllegalStateException("No such field: executionResult/lots", e);
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
                jsonArray.put(lot.toJSONObject());
            }
            body.put(LOTS, jsonArray);
            return new HttpResponse(StatusCode.OK, body);
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
