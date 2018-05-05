package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import utils.API;

public class BetResponseCommand extends SimpleResponseCommand {

    public BetResponseCommand(HttpResponse httpResponse) throws JSONException {
        super(httpResponse);
    }

    public BetResponseCommand(String executionResult) {
        super(executionResult);
    }

    @Override
    public API getAPI() {
        return API.BET;
    }
}
