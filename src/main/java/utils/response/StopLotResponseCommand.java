package utils.response;

import http.HttpResponse;
import utils.API;

public class StopLotResponseCommand extends SimpleResponseCommand {

    public StopLotResponseCommand(HttpResponse httpResponse) {
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
