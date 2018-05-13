package ru.spbau.bachelors2015.roulette.server

class NicknameIsTakenException: Exception()

class CroupierIsAlreadyRegisteredException: Exception()

class GameIsRunningException: Exception()

class CasinoModel {
    private var isCroupierRegistered = false

    private val nicknames = mutableSetOf<String>()

    private var freeGameId = 0

    private var previousGame: Game? = null

    private var currentGame: Game? = null

    fun registerPlayer(nickname: String): Player {
        synchronized(this) {
            if (nicknames.contains(nickname)) {
                throw NicknameIsTakenException()
            }

            nicknames.add(nickname)

            return PlayerImplementation(nickname)
        }
    }

    fun registerCroupier(nickname: String): Croupier {
        synchronized(this) {
            if (nicknames.contains(nickname)) {
                throw NicknameIsTakenException()
            }

            if (isCroupierRegistered) {
                throw CroupierIsAlreadyRegisteredException()
            }

            isCroupierRegistered = true
            nicknames.add(nickname)

            return CroupierImplementation(nickname)
        }
    }

    fun getCurrentGame(): Game? {
        return currentGame
    }

    fun getGame(id: Int): Game? {
        synchronized(this) {
            if (currentGame?.id == id) {
                return currentGame
            } else if (previousGame?.id == id) {
                return previousGame
            }

            return null
        }
    }

    fun startNewGame(): Game {
        synchronized(this) {
            if (currentGame?.isOver() == false) {
                throw GameIsRunningException()
            }

            previousGame = currentGame
            val newGame = Game(++freeGameId)
            currentGame = newGame

            return newGame
        }
    }

    interface Role {
        val nickname: String

        fun destroy()
    }

    interface Player : Role {
        fun getBalance() : Int

        fun addMoney(amount: Int)

        fun subtractMoney(amount: Int)
    }

    interface Croupier : Role

    private inner class PlayerImplementation(override val nickname: String) : Player {
        private var balance: Int = initialBalance

        override fun getBalance(): Int {
            synchronized(this) {
                return balance
            }
        }

        override fun addMoney(amount: Int) {
            synchronized(this) {
                balance += amount
            }
        }

        override fun subtractMoney(amount: Int) {
            synchronized(this) {
                balance -= amount
            }
        }

        override fun destroy() {
            synchronized(this@CasinoModel) {
                nicknames.remove(nickname)
            }
        }
    }

    private inner class CroupierImplementation(override val nickname: String) : Croupier {
        override fun destroy() {
            synchronized(this@CasinoModel) {
                isCroupierRegistered = false
                nicknames.remove(nickname)
            }
        }
    }

    companion object {
        const val initialBalance = 1000
    }
}
