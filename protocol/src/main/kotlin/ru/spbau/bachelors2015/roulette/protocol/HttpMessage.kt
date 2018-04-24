package ru.spbau.bachelors2015.roulette.protocol

/**
 * Super class of HttpRequest and HttpResponse
 */
abstract class HttpMessage(
    predefinedHeaders: Map<String, String>?,
    val messageBody: String?
) {
    val headers: Map<String, String>

    init {
        val tmp = predefinedHeaders?.toSortedMap() ?: sortedMapOf()

        if (messageBody != null) {
            tmp[HttpMessageElements.bodyLengthHeaderName] = messageBody.length.toString()
        }

        headers = tmp
    }

    protected fun stringRepresentationOfHeaders(): String {
        return buildString {
            if (headers.isNotEmpty()) {
                for (entry in headers) {
                    append(entry.key)
                    append(HttpMessageElements.headerKeyValueSeparator)
                    append(entry.value)
                    append(HttpMessageElements.newlineDelimiter)
                }
            }
        }
    }

    protected fun stringRepresentationOfBody(): String {
        return messageBody ?: ""
    }
}
