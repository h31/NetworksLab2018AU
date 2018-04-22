package utils.response;

import http.HttpResponse;

public class StopLotResponseCommand extends SimpleResponseCommand {

    public StopLotResponseCommand(HttpResponse httpResponse) {
        super(httpResponse);
    }

    public StopLotResponseCommand(String executionResult) {
        super(executionResult);
    }
}
