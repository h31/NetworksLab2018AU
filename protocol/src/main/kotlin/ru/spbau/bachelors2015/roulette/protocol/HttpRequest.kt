package ru.spbau.bachelors2015.roulette.protocol

import java.util.*

enum class HttpRequestMethod {
    GET, PUT, CONNECT
}

/**
 * A class that represent http request message and stores all the data that this message consists
 * of.
 */
class HttpRequest(
    val method: HttpRequestMethod,
    predefinedHeaders: Map<String, String>?,
    val uri: Uri,
    val messageBody: String?
): HttpMessage() {
    val headers: Map<String, String>

    init {
        val tmp = predefinedHeaders?.toSortedMap() ?: sortedMapOf()

        if (messageBody != null) {
            tmp[HttpMessageElements.bodyLengthHeaderName] = messageBody.length.toString()
        }

        headers = tmp
    }

    /**
     * Converts this http request to a string representation according to the protocol.
     */
    override fun toString(): String {
        return buildString {
            append(method)
            append(HttpMessageElements.spaceDelimiter)
            append(uri)
            append(HttpMessageElements.spaceDelimiter)
            append(HttpMessageElements.httpVersion)
            append(HttpMessageElements.newlineDelimiter)

            if (headers.isNotEmpty()) {
                for (entry in headers) {
                    append(entry.key)
                    append(HttpMessageElements.headerKeyValueSeparator)
                    append(entry.value)
                    append(HttpMessageElements.newlineDelimiter)
                }
            }

            append(HttpMessageElements.newlineDelimiter)

            if (messageBody != null) {
                append(messageBody)
            }
        }
    }
}
