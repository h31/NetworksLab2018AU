package network.requests

import network.http.HttpRequestMethod
import network.http.HttpRequest

class RegisterHttpRequest : HttpRequest(HttpRequestMethod.POST, "/register")
