package ru.spbau.bachelors2015.roulette.protocol

import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.equalTo
import org.hamcrest.MatcherAssert.assertThat
import org.junit.Test

class HttpRequestTest {
    @Test
    fun stringConversionTest() {
        val queryLine = QueryLine(mapOf(Pair("a", "b"), Pair("c", "d")))
        val uri = Uri(listOf("p", "a", "t", "h"), queryLine)
        val message = "Hello, world!"
        val httpRequest = HttpRequest(HttpRequestMethod.GET, uri, message)

        val expected =
            "GET p/a/t/h?a=b&c=d HTTP/1.1\r\n" +
            "${HttpMessageElements.bodyLengthHeaderName}:${message.length}\r\n\r\n" +
            message

        assertThat(httpRequest.toString(), `is`(equalTo(expected)))
    }
}