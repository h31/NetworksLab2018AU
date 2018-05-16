package ru.spbau.bachelors2015.roulette.protocol.http

import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.equalTo
import org.hamcrest.MatcherAssert.assertThat
import org.junit.Test
import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream

class HttpStreamsTest {
    @Test
    fun requestWriteReadTest() {
        val queryLine = QueryLine(mapOf(Pair("a", "b"), Pair("c", "d")))
        val uri = Uri(ResourcePath("p", "a", "t", "h"), queryLine)
        val message = "Hello, world!"
        val initialHttpRequest = HttpRequest(
                HttpRequestMethod.GET,
                mapOf(Pair("h", "1")),
                uri,
                message
        )

        val outputStream = ByteArrayOutputStream()

        HttpOutputStream(outputStream).use {
            it.write(initialHttpRequest)
        }

        val readHttpRequest =
            HttpInputStream(ByteArrayInputStream(outputStream.toByteArray())).use {
                it.readHttpRequest()
            }

        assertThat(readHttpRequest.toString(), `is`(equalTo(initialHttpRequest.toString())))
    }

    @Test
    fun responseWriteReadTest() {
        val message = "Hello, world!"
        val initialHttpResponse =
                HttpResponse(HttpResponseStatus.OK, mapOf(Pair("a", "b")), message)

        val outputStream = ByteArrayOutputStream()

        HttpOutputStream(outputStream).use {
            it.write(initialHttpResponse)
        }

        val readHttpResponse =
            HttpInputStream(ByteArrayInputStream(outputStream.toByteArray())).use {
                it.readHttpResponse()
            }

        assertThat(readHttpResponse.toString(), `is`(equalTo(initialHttpResponse.toString())))
    }
}