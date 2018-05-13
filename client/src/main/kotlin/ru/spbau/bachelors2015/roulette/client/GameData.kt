package ru.spbau.bachelors2015.roulette.client

import javafx.collections.FXCollections
import javafx.collections.ObservableList
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientRole

object GameData {
    var nickname: String? = null
    var gameId: Int? = null
    var role: ClientRole? = null
    val items: ObservableList<String> = FXCollections.observableArrayList()
}