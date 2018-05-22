package network.requests

import network.http.HttpRequestMethod

class PayHttpRequest(
    amount: Int,
    toId: Int,
    authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.POST, "/pay/$amount/$toId", authToken)
