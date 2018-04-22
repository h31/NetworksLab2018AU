package utils.response;

import http.HttpResponse;

public class RegisterResponseCommand extends SimpleResponseCommand {

    public RegisterResponseCommand(HttpResponse httpResponse) {
        super(httpResponse);
    }

    public RegisterResponseCommand(String executionResult) {
        super(executionResult);
    }
}
