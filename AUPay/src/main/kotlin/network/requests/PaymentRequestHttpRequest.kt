package network.requests

import network.http.HttpRequestMethod

class PaymentRequestHttpRequest(
        amount: Int,
        fromId: Int,
        authToken: String
) : AuthorizedHttpRequest(HttpRequestMethod.POST, "/request/$amount/$fromId", authToken)