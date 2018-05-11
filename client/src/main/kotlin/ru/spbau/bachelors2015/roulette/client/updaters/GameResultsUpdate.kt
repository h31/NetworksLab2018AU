package ru.spbau.bachelors2015.roulette.client.updaters

import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.client.handlers.GameResultsHandler
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientCommunicationSocket
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsRequest

class GameResultsUpdate(
        private val client: ClientCommunicationSocket,
        private val score: Label,
        private val rouletteValue: Label,
        private val playerPayout: Label
) : Runnable {
    override fun run() {
        val request = GameResultsRequest(GameData.gameId!!)

        client.send(request, GameResultsHandler(score, rouletteValue, playerPayout))
    }
}