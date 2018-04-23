package utils.response;

import http.HttpResponse;
import utils.API;

public class RegisterResponseCommand extends SimpleResponseCommand {

    public RegisterResponseCommand(HttpResponse httpResponse) {
        super(httpResponse);
    }

    public RegisterResponseCommand(String executionResult) {
        super(executionResult);
    }

    @Override
    public API getAPI() {
        return API.REGISTER;
    }
}
