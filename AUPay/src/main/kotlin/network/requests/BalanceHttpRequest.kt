package network.requests

import network.http.HttpRequestMethod

class BalanceHttpRequest(
    authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.GET, "/balance", authToken)
