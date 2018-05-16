package ru.spbau.bachelors2015.roulette.protocol.http

import org.junit.Test

class QueryLineTest {
    @Test(expected = IllegalArgumentException::class)
    fun emptyQueryLineCreation() {
        QueryLine(mapOf())
    }
}
