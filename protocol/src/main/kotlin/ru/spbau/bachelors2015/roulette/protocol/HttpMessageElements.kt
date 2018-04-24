package ru.spbau.bachelors2015.roulette.protocol

object HttpMessageElements {
    const val spaceDelimiter = ' '

    const val newlineDelimiter = "\r\n"

    const val headerKeyValueSeparator = ':'

    const val httpVersion = "HTTP/1.1"

    const val bodyLengthHeaderName = "body-length"

    val charset = Charsets.US_ASCII
}
