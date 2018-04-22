package utils.request;

import http.HttpRequest;
import utils.API;

public interface RequestCommand {

    HttpRequest toHttpRequest();

    API getAPI();
}
