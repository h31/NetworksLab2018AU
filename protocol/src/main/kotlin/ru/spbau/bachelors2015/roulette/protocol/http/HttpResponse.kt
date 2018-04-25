package ru.spbau.bachelors2015.roulette.protocol.http

enum class HttpResponseStatus(val statusCode: Int, val reasonPhrase: String) {
    OK(200, "OK"), BAD_REQUEST(400, "BadRequest");

    companion object {
        fun fromStatusCode(statusCode: Int): HttpResponseStatus {
            for (element in HttpResponseStatus.values()) {
                if (element.statusCode == statusCode) {
                    return element
                }
            }

            throw NoSuchElementException()
        }
    }
}

/**
 * A class that represent http response message and stores all the data that this message consists
 * of.
 */
class HttpResponse(
        val status: HttpResponseStatus,
        predefinedHeaders: Map<String, String>?,
        messageBody: String?
): HttpMessage(predefinedHeaders, messageBody) {
    /**
     * Converts this http response to a string representation according to the protocol.
     */
    override fun toString(): String {
        return buildString {
            append(HttpMessageElements.httpVersion)
            append(HttpMessageElements.spaceDelimiter)
            append(status.statusCode)
            append(HttpMessageElements.spaceDelimiter)
            append(status.reasonPhrase)
            append(HttpMessageElements.newlineDelimiter)

            append(stringRepresentationOfHeaders())
            append(HttpMessageElements.newlineDelimiter)
            append(stringRepresentationOfBody())
        }
    }
}
