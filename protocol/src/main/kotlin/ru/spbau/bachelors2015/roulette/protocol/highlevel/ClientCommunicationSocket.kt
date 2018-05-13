package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpInputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpOutputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponse
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
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(OkResponse.fromHttpRepresentation(response))

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )

                else -> throw ProtocolException("Unexpected response status")
            }
        }
    }

    fun send(request: GameStartRequest, handler: GameStartResponseHandler) {
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(
                        GameStartResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )

                else -> throw ProtocolException("Unexpected response status")
            }
        }
    }

    fun send(request: GameStatusRequest, handler: GameStatusResponseHandler) {
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(
                        GameStatusPositiveResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.NO_CONTENT -> handler.handle(
                        GameStatusNegativeResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )
            }
        }
    }

    fun send(request: BalanceRequest, handler: BalanceResponseHandler) {
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(
                        BalanceResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )

                else -> throw ProtocolException("Unexpected response status")
            }
        }
    }

    fun send(request: BetRequest, handler: BetResponseHandler) {
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(
                        OkResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )

                else -> throw ProtocolException("Unexpected response status")
            }
        }
    }

    fun send(request: GameResultsRequest, handler: GameResultsResponseHandler) {
        synchronized(this) {
            val response = sendRequest(request)

            when (response.status) {
                HttpResponseStatus.OK -> handler.handle(
                        GameResultsResponse.fromHttpRepresentation(response)
                )

                HttpResponseStatus.BAD_REQUEST -> handler.handle(
                        ErrorResponse.fromHttpRepresentation(response)
                )

                else -> throw ProtocolException("Unexpected response status")
            }
        }
    }

    /**
     * Closes underlying socket.
     */
    override fun close() {
        synchronized(this) {
            underlyingSocket.close()
        }
    }

    private fun sendRequest(request: Request): HttpResponse {
        outputStream.write(request.toHttpRepresentation())
        return inputStream.readHttpResponse()
    }
}
