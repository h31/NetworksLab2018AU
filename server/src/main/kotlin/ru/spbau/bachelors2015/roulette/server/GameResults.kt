package ru.spbau.bachelors2015.roulette.server

class GameResults(balanceChanges: Map<CasinoModel.Player, Int>) {
    val payout: Map<CasinoModel.Player, Int> = balanceChanges.toMap()
}
