package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.HttpInputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpOutputStream
import ru.spbau.bachelors2015.roulette.protocol.http.HttpResponseStatus
import java.io.Closeable
import java.net.Socket

interface RegistrationResponseHandler {
    fun handle(response: OkResponse)

    fun handle(response: ErrorResponse)
}

/**
 * Socket wrapper that allow client to send requests and receive corresponding responses.
 */
class ClientCommunicationSocket(private val underlyingSocket: Socket): Closeable {
    private val inputStream = HttpInputStream(underlyingSocket.getInputStream())

    private val outputStream = HttpOutputStream(underlyingSocket.getOutputStream())

    fun send(registrationRequest: RegistrationRequest, handler: RegistrationResponseHandler) {
        outputStream.write(registrationRequest.toHttpRepresentation())
        val response = inputStream.readHttpResponse()

        when (response.status) {
            HttpResponseStatus.OK -> handler.handle(OkResponse())

            HttpResponseStatus.BAD_REQUEST -> handler.handle(
                ErrorResponse(response.messageBody ?: "")
            )
        }
    }

    /**
     * Closes underlying socket.
     */
    override fun close() {
        underlyingSocket.close()
    }
}
