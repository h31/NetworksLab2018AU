package server

import java.util.*

sealed class ServerException(
        message: String? = null
): Exception(message)

class NoSuchUserTokenException(
        userAuthToken: UUID
): PaymentException("Auth token $userAuthToken matches no user")

class NoSuchUserIdException(
        userId: Int
): PaymentException("Id $userId matches no user")

sealed class PaymentException(
        message: String? = null
): ServerException(message)

class NoSuchPaymentException(
        userId: Int,
        paymentId: Int
): PaymentException("Payment $paymentId could not be found for user $userId")

class IllegalPaymentStateForOperation(
        paymentId: Int,
        operation: String
): PaymentException("Payment $paymentId state does not support \"$operation\"")

class IllegalUserForOperation(
        userId: Int,
        paymentId: Int,
        operation: String
): PaymentException("User $userId is not allowed to perform \"$operation\" on payment $paymentId")