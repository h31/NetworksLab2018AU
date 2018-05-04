package ru.spbau.bachelors2015.roulette.protocol.http

import java.io.*
import java.net.ProtocolException

/**
 * A wrapper of input stream which can read http messages.
 */
class HttpInputStream(inputStream: InputStream): Closeable {
    private val underlyingStream =
        BufferedReader(InputStreamReader(inputStream, HttpMessageElements.charset))

    /**
     * Reads http request from this stream.
     */
    fun readHttpRequest(): HttpRequest {
        val lines = readHeaderOfMessageAsLines()

        val (method, uri) = parseStartLineOfRequest(lines[0])
        val headers = parseHeaders(lines.drop(1))

        val body = handleBody(headers)

        return HttpRequest(method, headers, uri, body)
    }

    /**
     * Reads http response from this stream.
     */
    fun readHttpResponse(): HttpResponse {
        val lines = readHeaderOfMessageAsLines()

        val status = parseStartLineOfResponse(lines[0])
        val headers = parseHeaders(lines.drop(1))

        val body = handleBody(headers)

        return HttpResponse(status, headers, body)
    }

    /**
     * Closes underlying stream.
     */
    override fun close() {
        underlyingStream.close()
    }

    private fun readHeaderOfMessageAsLines(): List<String> {
        val lines = mutableListOf<String>()

        while (true) {
            val line = underlyingStream.readLine() ?: throw EOFException()

            if (line.isEmpty()) {
                break
            }

            lines.add(line)
        }

        if (lines.isEmpty()) {
            throw ProtocolException("Empty HTTP message")
        }

        return lines
    }

    private fun parseStartLineOfRequest(startLine: String): Pair<HttpRequestMethod, Uri> {
        val tokens = startLine.split(HttpMessageElements.spaceDelimiter)
        if (tokens.size != 3 || tokens[2] != HttpMessageElements.httpVersion) {
            throw ProtocolException("Invalid HTTP request start line")
        }

        return Pair(parseRequestMethod(tokens[0]), parseUri(tokens[1]))
    }

    private fun parseStartLineOfResponse(startLine: String): HttpResponseStatus {
        val tokens = startLine.split(HttpMessageElements.spaceDelimiter)
        if (tokens.size != 3 || tokens[0] != HttpMessageElements.httpVersion) {
            throw ProtocolException("Invalid HTTP response start line")
        }

        val statusCode = try {
            tokens[1].toInt()
        } catch (_: NumberFormatException) {
            throw ProtocolException("Invalid HTTP response status code")
        }

        return HttpResponseStatus.fromStatusCode(statusCode)
    }

    private fun parseRequestMethod(method: String): HttpRequestMethod {
        return try {
            HttpRequestMethod.valueOf(method)
        } catch (_: IllegalArgumentException) {
            throw ProtocolException("Invalid HTTP request method")
        }
    }

    private fun parseUri(uri: String): Uri {
        val tokens = uri.split(Uri.queryLineSeparator)
        if (tokens.size != 2) {
            throw ProtocolException("Invalid HTTP request URI")
        }

        return Uri(
            ResourcePath(tokens[0].split(ResourcePath.elementsSeparator)),
            parseQueryLine(tokens[1])
        )
    }

    private fun parseQueryLine(queryLine: String): QueryLine {
        try {
            return QueryLine(
                    parseKeyValuePairs(
                        queryLine.split(QueryLine.pairsSeparator),
                        QueryLine.keyValuePairSeparator
                    )
            )
        } catch (_: IllegalArgumentException) {
            throw ProtocolException("Invalid query line in HTTP request")
        }
    }

    private fun parseHeaders(headers: List<String>): MutableMap<String, String> {
        try {
            return parseKeyValuePairs(headers, HttpMessageElements.headerKeyValueSeparator)
        } catch (_: IllegalArgumentException) {
            throw ProtocolException("Invalid header in HTTP message")
        }
    }

    private fun handleBody(headers: MutableMap<String, String>): String? {
        if (!headers.containsKey(HttpMessageElements.bodyLengthHeaderName)) {
            return null
        }

        val bodyLength = try {
            headers[HttpMessageElements.bodyLengthHeaderName]!!.toInt()
        } catch (_: NumberFormatException) {
            throw ProtocolException("Invalid body length in HTTP message")
        }

        headers.remove(HttpMessageElements.bodyLengthHeaderName)

        val charArray = CharArray(bodyLength)
        underlyingStream.read(charArray)

        return charArray.joinToString("")
    }

    private fun parseOneKeyValuePair(pair: String, separator: Char): Pair<String, String> {
        val tokens = pair.split(separator)

        if (tokens.size != 2) {
            throw IllegalArgumentException("Key value pair string contains several separators")
        }

        val (key, value) = tokens
        return Pair(key, value)
    }

    private fun parseKeyValuePairs(
            headers: List<String>,
            separator: Char
    ): MutableMap<String, String> {
        return hashMapOf(*headers.map { parseOneKeyValuePair(it, separator) }.toTypedArray())
    }
}
