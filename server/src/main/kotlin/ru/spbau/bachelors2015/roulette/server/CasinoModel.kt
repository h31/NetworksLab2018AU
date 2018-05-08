package ru.spbau.bachelors2015.roulette.server

class NicknameIsTakenException: Exception()

class CroupierIsAlreadyRegisteredException: Exception()

class CasinoModel {
    private var isCroupierRegistered = false

    private val nicknames = mutableSetOf<String>()

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

    interface Role {
        val nickname: String

        fun destroy()
    }

    interface Player : Role

    interface Croupier : Role

    private inner class PlayerImplementation(override val nickname: String) : Player {
        override fun destroy() {
            // TODO: need to handle destruction during active game

            synchronized(this@CasinoModel) {
                nicknames.remove(nickname)
            }
        }
    }

    private inner class CroupierImplementation(override val nickname: String) : Croupier {
        override fun destroy() {
            // TODO: need to handle destruction during active game

            synchronized(this@CasinoModel) {
                isCroupierRegistered = false
                nicknames.remove(nickname)
            }
        }
    }
}
