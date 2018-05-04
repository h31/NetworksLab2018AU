package ru.spbau.bachelors2015.roulette.protocol.highlevel

import java.io.Closeable
import java.net.Socket

class ServerCommunicationSocket(private val underlyingSocket: Socket): Closeable {
    /**
     * Closes underlying socket.
     */
    override fun close() {
        underlyingSocket.close()
    }
}
