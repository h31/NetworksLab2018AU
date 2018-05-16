package ru.spbau.bachelors2015.roulette.server

import org.junit.rules.TestRule
import org.junit.runner.Description
import org.junit.runners.model.Statement

class ServerRule : TestRule {
    override fun apply(base: Statement?, description: Description?): Statement {
        return object: Statement() {
            override fun evaluate() {
                val server = Server(serverPort)
                server.start()

                try {
                    base?.evaluate()
                } finally {
                    server.stop()
                }
            }
        }
    }

    companion object {
        const val serverPort = 9000
    }
}