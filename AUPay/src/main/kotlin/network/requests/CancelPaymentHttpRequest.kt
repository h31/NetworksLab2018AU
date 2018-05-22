package network.requests

import network.http.HttpRequestMethod

class CancelPaymentHttpRequest(
        paymentId: Int,
        authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.POST, "/cancel/$paymentId", authToken)
