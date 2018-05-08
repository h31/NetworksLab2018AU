package ru.spbau.bachelors2015.roulette.server

import ru.spbau.bachelors2015.roulette.protocol.highlevel.*
import java.lang.Math.max
import java.util.*
import java.util.concurrent.TimeUnit

class GameIsOverException: Exception()

class GameIsNotOverException: Exception()

class BetHasAlreadyBeenMadeException: Exception()

class Game(val id: Int) {
    private val beginTime = System.nanoTime()

    private val boardSize = 36

    val value = Random().nextInt(boardSize) + 1

    private val bets = mutableMapOf<CasinoModel.Player, Bet>()

    fun getBets(): Map<CasinoModel.Player, Bet> {
        synchronized(this) {
            return bets.toMap()
        }
    }

    fun secondsLeft(): Int {
        val secondsPast = TimeUnit.SECONDS.convert(
            System.nanoTime() - beginTime,
            TimeUnit.NANOSECONDS
        )

        return max(0, 10 - secondsPast).toInt()
    }

    fun isOver(): Boolean {
        return secondsLeft() == 0
    }

    fun makeBet(player: CasinoModel.Player, bet: Bet) {
        if (isOver()) {
            throw GameIsOverException()
        }

        synchronized(this) {
            if (bets.containsKey(player)) {
                throw BetHasAlreadyBeenMadeException()
            }

            bets[player] = bet
        }
    }

    fun getResults(): Map<CasinoModel.Player, Int> {
        if (!isOver()) {
            throw GameIsNotOverException()
        }

        return bets.mapValues { (_, bet) ->
            if (bet.isWinningWith(value)) {
                bet.accept(object : BetVisitor<Int> {
                    override fun visit(bet: BetOnEvenNumbers): Int {
                        return 2 * bet.value
                    }

                    override fun visit(bet: BetOnOddNumbers): Int {
                        return 2 * bet.value
                    }

                    override fun visit(bet: BetOnExactNumber): Int {
                        return boardSize * bet.value
                    }
                })
            } else {
                0
            }
        }
    }
}
