package network.requests

import network.http.HttpRequestMethod

class DenyPaymentHttpRequest(
    paymentId: Int,
    authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.POST, "/deny/$paymentId", authToken)
