package network.requests

import network.http.HttpRequestMethod

class GetPaymentsHttpRequest(
    authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.GET, "/payments", authToken)
