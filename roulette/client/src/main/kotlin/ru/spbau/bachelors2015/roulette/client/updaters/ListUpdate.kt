package ru.spbau.bachelors2015.roulette.client.updaters

import ru.spbau.bachelors2015.roulette.client.handlers.GameStatusHandler
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientCommunicationSocket
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusRequest

class ListUpdate(
        private val client: ClientCommunicationSocket
) : Runnable {
    override fun run() {
        while (true) {
            val request = GameStatusRequest()
            client.send(request, GameStatusHandler())
            Thread.sleep(1000)
        }
    }
}