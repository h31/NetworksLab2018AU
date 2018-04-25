package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponse
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponseStatus

/**
 * Abstract class which represents a response from a server.
 */
abstract class Response {
    /**
     * Returns http representation of this response.
     */
    fun toHttpRepresentation(): HttpResponse {
        return HttpResponse(statusCode, null, messageBody)
    }

    protected abstract val statusCode: HttpResponseStatus

    protected abstract val messageBody: String?
}

/**
 * Error response.
 */
class ErrorResponse(public override val messageBody: String): Response() {
    public override val statusCode: HttpResponseStatus = HttpResponseStatus.BAD_REQUEST
}

/**
 * OK response that doesn't have any additional message.
 */
class OkResponse : Response() {
    public override val statusCode: HttpResponseStatus = HttpResponseStatus.OK

    override val messageBody: String? = null
}
