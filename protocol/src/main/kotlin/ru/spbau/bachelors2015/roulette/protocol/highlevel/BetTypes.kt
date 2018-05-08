package ru.spbau.bachelors2015.roulette.protocol.highlevel

interface BetTypeVisitor<T> {
    fun visit(betType: BetOnEvenNumbers): T

    fun visit(betType: BetOnOddNumbers): T

    fun visit(betType: BetOnExactNumber): T
}

abstract class Bet(val value: Int) {
    abstract fun isWinningWith(number: Int): Boolean

    abstract fun <T> accept(visitor: BetTypeVisitor<T>): T
}

class BetOnEvenNumbers(value: Int): Bet(value) {
    override fun isWinningWith(number: Int): Boolean {
        return number % 2 == 0
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}

class BetOnOddNumbers(value: Int): Bet(value) {
    override fun isWinningWith(number: Int): Boolean {
        return number % 2 == 1
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}

class BetOnExactNumber(val number: Int, value: Int): Bet(value) {
    override fun isWinningWith(number: Int): Boolean {
        return this.number == number
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}
