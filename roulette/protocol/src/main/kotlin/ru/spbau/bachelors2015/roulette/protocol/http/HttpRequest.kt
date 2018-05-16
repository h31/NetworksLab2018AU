package ru.spbau.bachelors2015.roulette.protocol.http

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
        messageBody: String?
): HttpMessage(predefinedHeaders, messageBody) {
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

            append(stringRepresentationOfHeaders())
            append(HttpMessageElements.newlineDelimiter)
            append(stringRepresentationOfBody())
        }
    }
}
