package network.data

import kotlinx.coroutines.experimental.sync.Mutex
import kotlinx.coroutines.experimental.sync.withLock
import org.json.JSONObject

data class Payment internal constructor(
        val id: Int,
        val fromId: Int,
        val toId: Int,
        val amount: Int,
        private var status: PaymentStatus
): Comparable<Payment> {
    private val statusMutex = Mutex()

    init {
        require(amount > 0)
    }

    override fun compareTo(other: Payment): Int = id.compareTo(other.id)

    suspend fun getStatus(): PaymentStatus =
            statusMutex.withLock { status }

    suspend fun setStatus(block: (PaymentStatus) -> PaymentStatus) =
            statusMutex.withLock { status = block(status) }

    suspend fun toJSONObject(): JSONObject =
            JSONObject()
                    .put("amount", amount)
                    .put("fromAccountId", fromId)
                    .put("paymentId", id)
                    .put("toAccountId", toId)
                    .put("status", getStatus())
}