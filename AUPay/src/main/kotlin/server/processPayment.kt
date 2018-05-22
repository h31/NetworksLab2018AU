package server

import network.data.Payment
import network.data.PaymentStatus

internal suspend fun processPayment(users: Iterable<User>, payment: Payment) {
    val fromUser = users.find { it.id == payment.fromId }
            ?: throw NoSuchUserIdException(payment.fromId)

    val toUser = users.find { it.id == payment.toId }
            ?: throw NoSuchUserIdException(payment.toId)

    payment.setStatus {
        return@setStatus when {
            !it.processable                   -> it
            fromUser.balance < payment.amount -> PaymentStatus.DENIED
            else                              -> {
                fromUser.balance -= payment.amount
                toUser.balance += payment.amount
                PaymentStatus.SUCCESSFUL
            }
        }
    }
}