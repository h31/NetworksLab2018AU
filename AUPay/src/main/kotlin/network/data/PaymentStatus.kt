package network.data

/**
 * Represents the status of the payment order:
 * -- PLACED means that the payment is in the system, but isn't confirmed or denied by fromAccountId
 * -- CONFIRMED means that the payment was confirmed by fromAccountId but not completed by the system
 * -- DENIED means that either the payment was denied by fromAccountId or there is not enough
 *    money on the balance
 * -- CANCELLED means that payment requester has cancelled the payment before it was conducted
 *    by the system
 * -- SUCCESSFUL means that the payment was conducted by the system
 */
enum class PaymentStatus(
        val cancellable: Boolean,
        val processable: Boolean
) {
    PLACED(true, true),
    CONFIRMED(true, true),
    DENIED(false, false),
    CANCELLED(false, false),
    SUCCESSFUL(false, false)
}
