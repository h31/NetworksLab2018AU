package ru.spbau.bachelors2015.roulette.protocol.highlevel

interface BetTypeVisitor<T> {
    fun visit(betType: BetOnEvenNumbers): T

    fun visit(betType: BetOnOddNumbers): T

    fun visit(betType: BetOnExactNumber): T
}

abstract class BetType {
    abstract fun isWinningWith(number: Int): Boolean

    abstract fun <T> accept(visitor: BetTypeVisitor<T>): T
}

class BetOnEvenNumbers: BetType() {
    override fun isWinningWith(number: Int): Boolean {
        return number % 2 == 0
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}

class BetOnOddNumbers: BetType() {
    override fun isWinningWith(number: Int): Boolean {
        return number % 2 == 1
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}

class BetOnExactNumber(val number: Int): BetType() {
    override fun isWinningWith(number: Int): Boolean {
        return this.number == number
    }

    override fun <T> accept(visitor: BetTypeVisitor<T>): T {
        return visitor.visit(this)
    }
}
