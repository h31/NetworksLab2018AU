package client.listeners

import client.ClientWindow
import network.requests.CancelPaymentHttpRequest
import java.awt.event.ActionEvent
import javax.swing.*

class CancelPaymentButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        try {
            val paymentIdToCancelField = JTextField("")
            val panel = JPanel()
            panel.layout = BoxLayout(panel, BoxLayout.Y_AXIS)
            panel.add(JLabel("Payment ID to cancel:"))
            panel.add(paymentIdToCancelField)
            val resultCode =
                JOptionPane.showConfirmDialog(
                    clientWindow,
                    panel,
                    "Please enter the payment ID",
                    JOptionPane.OK_CANCEL_OPTION)
            if (resultCode == JOptionPane.OK_OPTION) {
                val paymentIdToCancel = paymentIdToCancelField.text.toInt()
                val request = CancelPaymentHttpRequest(paymentIdToCancel, clientWindow.authToken!!)
                executeRequestInsideClientWindow(request) {
                    clientWindow.showMessageDialog("Payment was successfully cancelled.")
                }
            }
        } catch (_: Exception) {
            clientWindow.showMessageDialog("Incorrect input.")
        }
    }
}
