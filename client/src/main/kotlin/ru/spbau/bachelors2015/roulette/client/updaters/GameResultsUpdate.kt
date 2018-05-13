package ru.spbau.bachelors2015.roulette.client.updaters

import javafx.scene.control.Button
import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.client.handlers.GameResultsHandler
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientCommunicationSocket
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsRequest

class GameResultsUpdate(
        private val client: ClientCommunicationSocket,
        private val rouletteValue: Label,
        private val playerPayout: Label,
        private val makeBetButton: Button
) : Runnable {
    override fun run() {
        while (GameData.secondsLeft != 0) {
            Thread.sleep(2000)
            continue
        }

        val request = GameResultsRequest(GameData.gameId!!)
        client.send(request, GameResultsHandler(rouletteValue, playerPayout, makeBetButton))
    }
}