package network.requests

import network.http.HttpRequestMethod
import network.http.HttpRequest

class AccountsHttpRequest : HttpRequest(HttpRequestMethod.GET, "/accounts")
