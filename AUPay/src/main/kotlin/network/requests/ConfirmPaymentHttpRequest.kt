package network.requests

import network.http.HttpRequestMethod

class ConfirmPaymentHttpRequest(
    paymentId: Int,
    authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.POST, "/confirm/$paymentId", authToken)
