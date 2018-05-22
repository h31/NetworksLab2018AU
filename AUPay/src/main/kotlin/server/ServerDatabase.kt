package server

import kotlinx.coroutines.experimental.channels.SendChannel
import kotlinx.coroutines.experimental.channels.actor
import network.data.Payment
import network.data.PaymentStatus
import network.http.HttpRequest
import network.http.HttpResponse
import org.json.JSONArray
import org.json.JSONObject
import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.atomic.AtomicInteger

internal class ServerDatabase {
    private val users: ConcurrentLinkedQueue<User> = ConcurrentLinkedQueue()
    private val paymentQueue: SendChannel<Payment> = actor(capacity = Int.MAX_VALUE) {
        for (payment in channel) {
            processPayment(users, payment)
        }
    }

    internal fun processRegister(): HttpResponse {
        val user = newUser()
        users += user

        return HttpResponse(
                HttpResponse.OK_STATUS_CODE,
                "OK",
                mapOf("Set-Cookie" to "AuthToken=${user.authToken}"),
                JSONObject().put("accountId", user.id.toString())
        )
    }

    internal fun processAccounts(): HttpResponse {
        return HttpResponse.okFromBody(
                JSONObject().put(
                        "accounts",
                        JSONArray(users.map { JSONObject().put("id", it.id) })
                )
        )
    }

    internal fun User.processBalance(): HttpResponse =
            HttpResponse.okFromBody(
                    JSONObject().put("balance", balance)
            )

    internal suspend fun User.processPay(amount: Int, toId: Int): HttpResponse {
        val toUser = users.find { it.id == toId }
                ?: throw NoSuchUserIdException(toId)

        val newPayment = newPayment(
                id, toId, amount
        )

        payments += newPayment
        toUser.payments += newPayment
        paymentQueue.send(newPayment)

        return HttpResponse.okFromBody(
                JSONObject().put("paymentId", newPayment.id)
        )
    }

    internal suspend fun User.processCancel(paymentId: Int): HttpResponse {
        cancel(paymentId)

        return HttpResponse.okFromBody()
    }

    internal suspend fun User.processPayments(): HttpResponse {
        return HttpResponse.okFromBody(
                JSONObject().put(
                        "payments",
                        JSONArray(payments().map { it.toJSONObject() })
                )
        )
    }

    internal fun User.processRequestPayment(amount: Int, fromId: Int): HttpResponse {
        val toUser = users.find { it.id == fromId }
                ?: throw NoSuchUserIdException(fromId)

        val newPayment = newPayment(
                fromId, id, amount
        )

        paymentRequests += newPayment
        toUser.paymentRequests += newPayment

        return HttpResponse.okFromBody(
                JSONObject().put("paymentId", newPayment.id)
        )
    }

    internal suspend fun User.processConfirm(paymentId: Int): HttpResponse {
        paymentQueue.send(confirm(paymentId))

        return HttpResponse.okFromBody()
    }

    internal suspend fun User.processDeny(paymentId: Int): HttpResponse {
        deny(paymentId)

        return HttpResponse.okFromBody()
    }

    internal suspend fun HttpRequest.withAuthToken(
            authenticatedAction: suspend User.() -> HttpResponse
    ): HttpResponse {
        require(headers.containsKey("Cookie"))

        val cookie = headers["Cookie"]!!

        val equalsIndex = cookie.indexOf('=')
        require(equalsIndex != -1)

        val cookieName = cookie.substring(0, equalsIndex)
        require(cookieName == "AuthToken")

        val cookieValue = cookie.substring(equalsIndex + 1)
        require(cookieValue.isNotEmpty())

        val userAuthToken = UUID.fromString(cookieValue)
        val user = users.find { it.authToken == userAuthToken }
                ?: throw NoSuchUserTokenException(userAuthToken)

        return user.authenticatedAction()
    }

    private companion object {
        private var currentPaymentId: AtomicInteger = AtomicInteger(0)
        private var currentUserId: AtomicInteger = AtomicInteger(0)
        private const val INITIAL_BALANCE = 15000

        private fun newPayment(fromId: Int, toId: Int, amount: Int): Payment =
                Payment(
                        currentPaymentId.getAndIncrement(),
                        fromId,
                        toId,
                        amount,
                        PaymentStatus.PLACED
                )

        private fun newUser(): User =
                User(
                        currentUserId.getAndIncrement(),
                        UUID.randomUUID(),
                        INITIAL_BALANCE,
                        ConcurrentLinkedQueue(),
                        ConcurrentLinkedQueue()
                )
    }
}