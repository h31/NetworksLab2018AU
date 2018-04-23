package utils.response;

import http.HttpResponse;
import utils.API;

public class BetResponseCommand extends SimpleResponseCommand {

    public BetResponseCommand(HttpResponse httpResponse) {
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
