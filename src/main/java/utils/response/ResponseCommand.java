package utils.response;

import http.HttpResponse;
import utils.API;

public interface ResponseCommand {

    HttpResponse toHttpResponse();

    API getAPI();

    String getExecutionResult();
}
