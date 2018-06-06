package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import utils.API;

public class RegisterResponseCommand extends SimpleResponseCommand {

    public RegisterResponseCommand(HttpResponse httpResponse) throws JSONException {
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
