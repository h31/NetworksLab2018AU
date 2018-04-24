package ru.spbau.bachelors2015.roulette.protocol

import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.equalTo
import org.hamcrest.MatcherAssert
import org.junit.Test

class HttpResponseTest {
    @Test
    fun stringConversionTest() {
        val message = "Hello, world!"
        val httpResponse = HttpResponse(HttpResponseStatus.OK, mapOf(Pair("a", "b")), message)

        val expected =
            "HTTP/1.1 200 OK\r\n" +
            "a:b\r\n" +
            "${HttpMessageElements.bodyLengthHeaderName}:${message.length}\r\n\r\n" +
            message

        MatcherAssert.assertThat(httpResponse.toString(), `is`(equalTo(expected)))
    }
}