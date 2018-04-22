package utils.response;

import http.HttpResponse;
import utils.API;

public interface ResponseCommand {
    String EXECUTION_RESULT = "executionResult";

    HttpResponse toHttpResponse();

    API getAPI();

    String getExecutionResult();
}
