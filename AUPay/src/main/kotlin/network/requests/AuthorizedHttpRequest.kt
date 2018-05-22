package network.requests

import network.http.HttpRequestMethod
import network.http.HttpRequest

open class AuthorizedHttpRequest(
    method: HttpRequestMethod,
    path: String,
    authToken: String
) : HttpRequest(method, path, mapOf(Pair("Cookie", "AuthToken=$authToken")))
