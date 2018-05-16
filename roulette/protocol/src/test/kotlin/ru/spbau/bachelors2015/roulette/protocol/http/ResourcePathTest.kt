package ru.spbau.bachelors2015.roulette.protocol.http

import org.junit.Test

class ResourcePathTest {
    @Test(expected = IllegalArgumentException::class)
    fun pathWithEmptyElement() {
        ResourcePath(listOf(""))
    }
}
