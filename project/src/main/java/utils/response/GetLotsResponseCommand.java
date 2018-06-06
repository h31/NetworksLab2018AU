package utils.response;

import http.HttpResponse;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.API;
import utils.data.Lot;

import java.util.ArrayList;
import java.util.List;

public class GetLotsResponseCommand implements ResponseCommand {
    private static final String LOTS = "LOTS";
    private final String executionResult;
    private final List<Lot> lots;

    public GetLotsResponseCommand(HttpResponse httpResponse) throws JSONException {
        JSONObject body = httpResponse.getBody();
        System.out.println(body);
        lots = new ArrayList<>();
        JSONArray jsonArray = body.getJSONArray(LOTS);
        for (int i = 0; i < jsonArray.length(); i++) {
            JSONObject jsonObject = jsonArray.getJSONObject(i);
            Lot lot = new Lot(jsonObject);
            lots.add(lot);
        }
        executionResult = body.getString(EXECUTION_RESULT);
    }

    public GetLotsResponseCommand(String executionResult, List<Lot> lots) {
        this.executionResult = executionResult;
        this.lots = lots;
    }

    @Override
    public HttpResponse toHttpResponse() throws JSONException {
        JSONObject body = new JSONObject().put(EXECUTION_RESULT, executionResult);
        JSONArray jsonArray = new JSONArray();
        for (Lot lot: lots) {
            jsonArray.put(lot.toJSONObject());
        }
        body.put(LOTS, jsonArray);
        return new HttpResponse(StatusCode.OK, body);
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
