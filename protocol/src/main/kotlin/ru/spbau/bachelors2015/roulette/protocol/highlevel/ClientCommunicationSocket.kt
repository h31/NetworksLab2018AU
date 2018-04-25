package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpInputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpOutputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponseStatus
import java.io.Closeable
import java.net.ProtocolException
import java.net.Socket

interface RegistrationResponseHandler {
    fun handle(response: OkResponse)

    fun handle(response: ErrorResponse)
}

interface GameStartResponseHandler {
    fun handle(response: GameStartResponse)

    fun handle(response: ErrorResponse)
}

interface GameStatusResponseHandler {
    fun handle(response: GameStatusPositiveResponse)

    fun handle(response: GameStatusNegativeResponse)

    fun handle(response: ErrorResponse)
}

interface BalanceResponseHandler {
    fun handle(response: BalanceResponse)

    fun handle(response: ErrorResponse)
}

interface BetResponseHandler {
    fun handle(response: OkResponse)

    fun handle(response: ErrorResponse)
}

interface GameResultsResponseHandler {
    fun handle(response: GameResultsResponse)

    fun handle(response: ErrorResponse)
}

/**
 * Socket wrapper that allow client to send requests and receive corresponding responses.
 */
class ClientCommunicationSocket(private val underlyingSocket: Socket): Closeable {
    private val inputStream = HttpInputStream(underlyingSocket.getInputStream())

    private val outputStream = HttpOutputStream(underlyingSocket.getOutputStream())

    fun send(request: RegistrationRequest, handler: RegistrationResponseHandler) {
        outputStream.write(request.toHttpRepresentation())
        val response = inputStream.readHttpResponse()

        when (response.status) {
            HttpResponseStatus.OK -> handler.handle(OkResponse())

            HttpResponseStatus.BAD_REQUEST -> handler.handle(
                ErrorResponse(response.messageBody ?: "")
            )

            HttpResponseStatus.NO_CONTENT -> throw ProtocolException("Unexpected response status")
        }
    }

    fun send(request: GameStartRequest, handler: GameStartResponseHandler) {
        TODO("Implement")
    }

    fun send(request: GameStatusRequest, handler: GameStatusResponseHandler) {
        TODO("Implement")
    }

    fun send(request: BalanceRequest, handler: BalanceResponseHandler) {
        TODO("Implement")
    }

    fun send(request: BetRequest, handler: BetResponseHandler) {
        TODO("Implement")
    }

    fun send(request: GameResultsRequest, handler: GameResultsResponseHandler) {
        TODO("Implement")
    }

    /**
     * Closes underlying socket.
     */
    override fun close() {
        underlyingSocket.close()
    }
}
