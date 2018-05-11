package ru.spbau.bachelors2015.roulette.client.handlers

import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.client.BALANCE_PREFIX
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.protocol.highlevel.*

class BalanceHandler(private val balance: Label) : BalanceResponseHandler {
    override fun handle(response: BalanceResponse) {
        balance.text = BALANCE_PREFIX + response.balance.toString()
    }

    override fun handle(response: ErrorResponse) {

    }
}