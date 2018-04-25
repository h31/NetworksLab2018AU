package ru.spbau.bachelors2015.roulette.protocol.http

import java.io.Closeable
import java.io.OutputStream
import java.io.OutputStreamWriter

/**
 * A wrapper of output stream which can write http messages.
 */
class HttpOutputStream(outputStream: OutputStream): Closeable {
    private val underlyingStream = OutputStreamWriter(outputStream, HttpMessageElements.charset)

    /**
     * Write a given http message to this stream.
     */
    fun write(message: HttpMessage) {
        underlyingStream.write(message.toString())
    }

    /**
     * Closes underlying stream.
     */
    override fun close() {
        underlyingStream.close()
    }
}
