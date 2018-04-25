package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpMessageElements
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
class OkResponse: Response() {
    public override val statusCode: HttpResponseStatus = HttpResponseStatus.OK

    override val messageBody: String? = null
}

class GameStartResponse(val gameId: Int): Response() {
    override val statusCode = HttpResponseStatus.OK

    override val messageBody = gameId.toString()
}

class GameStatusPositiveResponse(
    val gameId: Int,
    val secondsLeft: Int,
    val bets: Map<String, Int>
): Response() {
    override val statusCode = HttpResponseStatus.OK

    override val messageBody = buildString {
        append(gameId)
        append(HttpMessageElements.spaceDelimiter)
        append(secondsLeft)
        append(HttpMessageElements.newlineDelimiter)

        for ((nickname, bet) in bets) {
            append(nickname)
            append(HttpMessageElements.spaceDelimiter)
            append(bet)
            append(HttpMessageElements.newlineDelimiter)
        }
    }
}

/**
 * Response which is returned when there is no running game at the moment.
 */
class GameStatusNegativeResponse: Response() {
    override val statusCode = HttpResponseStatus.NO_CONTENT

    override val messageBody = null
}

class BalanceResponse(val balance: Int): Response() {
    override val statusCode = HttpResponseStatus.OK

    override val messageBody = balance.toString()
}

class GameResultsResponse(
    val rouletteValue: Int,
    val balanceChanges: Map<String, Int>
): Response() {
    override val statusCode = HttpResponseStatus.OK

    override val messageBody = buildString {
        append(rouletteValue)
        append(HttpMessageElements.newlineDelimiter)

        for ((nickname, balanceChange) in balanceChanges) {
            append(nickname)
            append(balanceChange)
            append(HttpMessageElements.newlineDelimiter)
        }
    }
}
