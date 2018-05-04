package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpInputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpOutputStream
import java.io.Closeable
import java.net.Socket

interface RequestHandler {
    fun handle(request: RegistrationRequest): Response

    fun handle(request: GameStartRequest): Response

    fun handle(request: GameStatusRequest): Response

    fun handle(request: BalanceRequest): Response

    fun handle(request: BetRequest): Response

    fun handle(request: GameResultsRequest): Response

    fun handleSocketClosing()
}

// TODO: need to handle socket closing
class ServerCommunicationSocket(private val underlyingSocket: Socket): Closeable {
    private val inputStream = HttpInputStream(underlyingSocket.getInputStream())

    private val outputStream = HttpOutputStream(underlyingSocket.getOutputStream())

    fun handleNextRequest(handler: RequestHandler) {
        val request = inputStream.readHttpRequest()

        val response = try {
            when (request.uri.path) {
                RegistrationRequest.resourcePath ->
                    handler.handle(RegistrationRequest.fromHttpRepresentation(request))

                GameStartRequest.resourcePath ->
                    handler.handle(GameStartRequest.fromHttpRepresentation(request))

                GameStatusRequest.resourcePath ->
                    handler.handle(GameStatusRequest.fromHttpRepresentation(request))

                BalanceRequest.resourcePath ->
                    handler.handle(BalanceRequest.fromHttpRepresentation(request))

                BetRequest.resourcePath ->
                    handler.handle(BetRequest.fromHttpRepresentation(request))

                GameResultsRequest.resourcePath ->
                    handler.handle(GameResultsRequest.fromHttpRepresentation(request))

                else -> ErrorResponse("Unknown resource")
            }
        } catch (_: InvalidHttpRequest) {
            ErrorResponse("Invalid request format")
        }

        outputStream.write(response.toHttpRepresentation())
    }

    /**
     * Closes underlying socket.
     */
    override fun close() {
        underlyingSocket.close()
    }
}
