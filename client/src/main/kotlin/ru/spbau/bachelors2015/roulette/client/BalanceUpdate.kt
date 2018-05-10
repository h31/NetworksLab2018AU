package ru.spbau.bachelors2015.roulette.client

import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.protocol.highlevel.BalanceRequest
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientCommunicationSocket

class BalanceUpdate(
        private val client: ClientCommunicationSocket,
        private val balance: Label
) : Runnable {
    override fun run() {
        val request = BalanceRequest()
        client.send(request, BalanceHandler(balance))
    }
}