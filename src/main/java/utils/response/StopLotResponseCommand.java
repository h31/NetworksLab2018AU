package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import utils.API;

public class StopLotResponseCommand extends SimpleResponseCommand {

    public StopLotResponseCommand(HttpResponse httpResponse) throws JSONException {
        super(httpResponse);
    }

    public StopLotResponseCommand(String executionResult) {
        super(executionResult);
    }

    @Override
    public API getAPI() {
        return API.STOP_LOT;
    }
}
