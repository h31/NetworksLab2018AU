package utils.response;

import http.HttpResponse;
import org.json.JSONException;
import utils.API;

public interface ResponseCommand {
    String EXECUTION_RESULT = "executionResult";

    HttpResponse toHttpResponse() throws JSONException;

    API getAPI();

    String getExecutionResult();
}
