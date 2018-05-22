package server

import network.data.Payment
import network.data.PaymentStatus
import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue

internal data class User(
        val id: Int,
        val authToken: UUID,
        @Volatile var balance: Int,
        val payments: ConcurrentLinkedQueue<Payment>,
        val paymentRequests: ConcurrentLinkedQueue<Payment>
) {
    init {
        require(balance >= 0)
    }

    fun payments(): List<Payment> =
            merge(payments, paymentRequests)

    suspend fun cancel(paymentId: Int) {
        val payment = payments.find { it.id == paymentId }

        if (payment != null) {
            when {
                payment.fromId != id ->
                    throw IllegalUserForOperation(id, paymentId, "cancel")
                else                 -> {
                    payment.setStatus {
                        if (it.cancellable) {
                            return@setStatus PaymentStatus.CANCELLED
                        } else {
                            throw IllegalPaymentStateForOperation(paymentId, "cancel")
                        }
                    }
                    return
                }
            }
        }

        val request = paymentRequests.find { it.id == paymentId }

        if (request != null) {
            when {
                request.toId != id ->
                    throw IllegalUserForOperation(id, paymentId, "cancel")
                else               -> {
                    request.setStatus {
                        if (it.cancellable) {
                            return@setStatus PaymentStatus.CANCELLED
                        } else {
                            throw IllegalPaymentStateForOperation(paymentId, "cancel")
                        }
                    }
                    return
                }
            }
        }

        throw NoSuchPaymentException(id, paymentId)
    }

    suspend fun confirm(paymentId: Int): Payment {
        val request = paymentRequests.find { it.id == paymentId }

        if (request != null) {
            when {
                request.fromId != id ->
                    throw IllegalUserForOperation(id, paymentId, "confirm")
                else                 -> {
                    request.setStatus {
                        if (it == PaymentStatus.PLACED) {
                            return@setStatus PaymentStatus.CONFIRMED
                        } else {
                            throw IllegalPaymentStateForOperation(paymentId, "confirm")
                        }
                    }
                    return request
                }
            }
        }

        throw NoSuchPaymentException(id, paymentId)
    }

    suspend fun deny(paymentId: Int) {
        val request = paymentRequests.find { it.id == paymentId }

        if (request != null) {
            when {
                request.fromId != id                   ->
                    throw IllegalUserForOperation(id, paymentId, "deny")
                else                 -> {
                    request.setStatus {
                        if (it == PaymentStatus.PLACED) {
                            return@setStatus PaymentStatus.DENIED
                        } else {
                            throw IllegalPaymentStateForOperation(paymentId, "deny")
                        }
                    }
                    return
                }
            }
        }

        throw NoSuchPaymentException(id, paymentId)
    }
}