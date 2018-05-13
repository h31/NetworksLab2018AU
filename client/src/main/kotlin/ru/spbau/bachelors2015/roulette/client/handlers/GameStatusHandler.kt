package ru.spbau.bachelors2015.roulette.client.handlers

import javafx.collections.FXCollections
import javafx.collections.ObservableList
import javafx.scene.control.ListView
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusNegativeResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusPositiveResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusResponseHandler

class GameStatusHandler(private val listView: ListView<String>): GameStatusResponseHandler {
    private val items: ObservableList<String> = FXCollections.observableArrayList()

    override fun handle(response: GameStatusPositiveResponse) {
        GameData.gameId = response.gameId
        val seconds = "Seconds left : " + response.secondsLeft
        items.add(seconds)
        for (entry in response.bets) {
            val label = entry.key + " with bet: " + entry.value
            items.add(label)
        }

        listView.items = items
    }

    override fun handle(response: GameStatusNegativeResponse) {
    }

    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("cannot get game status")
    }
}