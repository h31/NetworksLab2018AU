package ru.spbau.bachelors2015.roulette.protocol.highlevel

import org.apache.commons.io.IOUtils
import ru.spbau.bachelors2015.roulette.protocol.http.HttpMessageElements
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponse
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponseStatus
import java.lang.NumberFormatException
import java.util.*

class InvalidHttpResponse : Exception()

/**
 * Abstract class which represents a response from a server.
 */
abstract class Response {
    /**
     * Returns http representation of this response.
     */
    fun toHttpRepresentation(): HttpResponse {
        return HttpResponse(status, null, messageBody)
    }

    protected abstract val status: HttpResponseStatus

    protected abstract val messageBody: String?
}

/**
 * Error response.
 */
class ErrorResponse(public override val messageBody: String): Response() {
    override val status: HttpResponseStatus = HttpResponseStatus.BAD_REQUEST

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): ErrorResponse {
            if (response.status != HttpResponseStatus.BAD_REQUEST) {
                throw InvalidHttpResponse()
            }

            return ErrorResponse(response.messageBody ?: "")
        }
    }
}

/**
 * OK response that doesn't have any additional message.
 */
class OkResponse: Response() {
    override val status: HttpResponseStatus = HttpResponseStatus.OK

    override val messageBody: String? = null

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): OkResponse {
            if (response.status != HttpResponseStatus.OK || response.messageBody != null) {
                throw InvalidHttpResponse()
            }

            return OkResponse()
        }
    }
}

class GameStartResponse(val gameId: Int): Response() {
    override val status = HttpResponseStatus.OK

    override val messageBody = gameId.toString()

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): GameStartResponse {
            if (response.status != HttpResponseStatus.OK || response.messageBody == null) {
                throw InvalidHttpResponse()
            }

            try {
                return GameStartResponse(response.messageBody.toInt())
            } catch (_: NumberFormatException) {
                throw InvalidHttpResponse()
            }
        }
    }
}

class GameStatusPositiveResponse(
    val gameId: Int,
    val secondsLeft: Int,
    val bets: Map<String, Int>
): Response() {
    override val status = HttpResponseStatus.OK

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

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): GameStatusPositiveResponse {
            if (response.status != HttpResponseStatus.OK || response.messageBody == null) {
                throw InvalidHttpResponse()
            }

            try {
                return Scanner(
                    IOUtils.toInputStream(response.messageBody, HttpMessageElements.charset),
                    HttpMessageElements.charset.name()
                ).use {
                    val gameId = it.nextInt()
                    val secondsLeft = it.nextInt()
                    val bets = mutableMapOf<String, Int>()

                    while (it.hasNext()) {
                        val nickname = it.next()
                        val bet = it.nextInt()

                        bets[nickname] = bet
                    }

                    return@use GameStatusPositiveResponse(gameId, secondsLeft, bets)
                }
            } catch (_: InputMismatchException) {
                throw InvalidHttpResponse()
            } catch (_: NoSuchElementException) {
                throw InvalidHttpResponse()
            }
        }
    }
}

/**
 * Response which is returned when there is no running game at the moment.
 */
class GameStatusNegativeResponse: Response() {
    override val status = HttpResponseStatus.NO_CONTENT

    override val messageBody = null

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): GameStatusNegativeResponse {
            if (response.status != HttpResponseStatus.NO_CONTENT || response.messageBody != null) {
                throw InvalidHttpResponse()
            }

            return GameStatusNegativeResponse()
        }
    }
}

class BalanceResponse(val balance: Int): Response() {
    override val status = HttpResponseStatus.OK

    override val messageBody = balance.toString()

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): BalanceResponse {
            if (response.status != HttpResponseStatus.OK || response.messageBody == null) {
                throw InvalidHttpResponse()
            }

            try {
                return BalanceResponse(response.messageBody.toInt())
            } catch (_: NumberFormatException) {
                throw InvalidHttpResponse()
            }
        }
    }
}

class GameResultsResponse(
    val rouletteValue: Int,
    val balanceChanges: Map<String, Int>
): Response() {
    override val status = HttpResponseStatus.OK

    override val messageBody = buildString {
        append(rouletteValue)
        append(HttpMessageElements.newlineDelimiter)

        for ((nickname, balanceChange) in balanceChanges) {
            append(nickname)
            append(HttpMessageElements.spaceDelimiter)
            append(balanceChange)
            append(HttpMessageElements.newlineDelimiter)
        }
    }

    companion object {
        fun fromHttpRepresentation(response: HttpResponse): GameResultsResponse {
            if (response.status != HttpResponseStatus.OK || response.messageBody == null) {
                throw InvalidHttpResponse()
            }

            try {
                return Scanner(
                    IOUtils.toInputStream(response.messageBody, HttpMessageElements.charset),
                    HttpMessageElements.charset.name()
                ).use {
                    val rouletteValue = it.nextInt()
                    val balanceChanges = mutableMapOf<String, Int>()

                    while (it.hasNext()) {
                        val nickname = it.next()
                        val balanceChange = it.nextInt()

                        balanceChanges[nickname] = balanceChange
                    }

                    return@use GameResultsResponse(rouletteValue, balanceChanges)
                }
            } catch (_: InputMismatchException) {
                throw InvalidHttpResponse()
            } catch (_: NoSuchElementException) {
                throw InvalidHttpResponse()
            }
        }
    }
}
