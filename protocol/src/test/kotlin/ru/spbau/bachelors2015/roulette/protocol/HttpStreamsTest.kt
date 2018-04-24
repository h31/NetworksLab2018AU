package ru.spbau.bachelors2015.roulette.protocol

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
        val uri = Uri(listOf("p", "a", "t", "h"), queryLine)
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
}