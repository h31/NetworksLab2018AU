package utils.response;

import http.HttpResponse;

public class BetResponseCommand extends SimpleResponseCommand {

    public BetResponseCommand(HttpResponse httpResponse) {
        super(httpResponse);
    }

    public BetResponseCommand(String executionResult) {
        super(executionResult);
    }
}
