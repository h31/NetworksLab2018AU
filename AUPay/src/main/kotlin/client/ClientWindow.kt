package client

import client.listeners.*
import network.SERVER_HOSTNAME
import network.SERVER_PORT
import java.awt.Component
import java.awt.GridLayout
import java.awt.Point
import javax.swing.*

class ClientWindow : JFrame("AUPay") {
    @Volatile var authToken: String? = null
    @Volatile var accountId: Int? = null

    val httpClient: HttpClient = HttpClient(SERVER_HOSTNAME, SERVER_PORT)

    private val registerButton = JButton("Register")

    init {
        setBounds(100, 100, 720, 480)
        defaultCloseOperation = JFrame.EXIT_ON_CLOSE
        contentPane.layout = BoxLayout(contentPane, BoxLayout.Y_AXIS)
        add(registerButton)
        registerButton.addActionListener(RegisterButtonListener(this))
    }

    fun proceedAfterSuccessfulRegistration() {
        registerButton.isEnabled = false

        val accountLabel = JLabel("Your account id is $accountId")
        accountLabel.alignmentX = Component.LEFT_ALIGNMENT
        add(accountLabel)

        val balanceButton = JButton("Check balance")
        balanceButton.addActionListener(BalanceButtonListener(this))
        balanceButton.alignmentX = Component.LEFT_ALIGNMENT
        add(balanceButton)

        val accountsButton = JButton("Get accounts")
        accountsButton.addActionListener(AccountsButtonListener(this))
        accountsButton.alignmentX = Component.LEFT_ALIGNMENT
        add(accountsButton)

        val payPanel = JPanel()
        payPanel.layout = BoxLayout(payPanel, BoxLayout.X_AXIS)
        val sendMoneyButton = JButton("Send money")
        sendMoneyButton.addActionListener(SendMoneyButtonListener(this))
        payPanel.add(sendMoneyButton)
        val askForMoneyButton = JButton("Ask for money")
        askForMoneyButton.addActionListener(AskForMoneyButtonListener(this))
        payPanel.add(askForMoneyButton)
        payPanel.alignmentX = Component.LEFT_ALIGNMENT
        add(payPanel)

        val getPaymentsButton = JButton("Get my payments")
        getPaymentsButton.addActionListener(GetPaymentsButtonListener(this))
        getPaymentsButton.alignmentX = Component.LEFT_ALIGNMENT
        add(getPaymentsButton)

        val commandPanel = JPanel()
        commandPanel.layout = BoxLayout(commandPanel, BoxLayout.X_AXIS)
        val confirmPaymentButton = JButton("Confirm payment")
        confirmPaymentButton.addActionListener(ConfirmPaymentButtonListener(this))
        commandPanel.add(confirmPaymentButton)
        val denyPaymentButton = JButton("Deny payment")
        denyPaymentButton.addActionListener(DenyPaymentButtonListener(this))
        commandPanel.add(denyPaymentButton)
        val cancelPaymentButton = JButton("Cancel payment")
        cancelPaymentButton.addActionListener(CancelPaymentButtonListener(this))
        commandPanel.add(cancelPaymentButton)
        commandPanel.alignmentX = Component.LEFT_ALIGNMENT
        add(commandPanel)

        revalidate()
    }

    fun showUnsuccessfulRequest(statusCode: Int, statusMessage: String) {
        JOptionPane.showMessageDialog(this, "$statusCode: $statusMessage")
    }

    fun showMessageDialog(message: String) {
        JOptionPane.showMessageDialog(this, message)
    }

    fun presentJTable(table: JTable) {
        val panel = JPanel()
        panel.layout = GridLayout()
        panel.add(JScrollPane(table))

        val dialog = JDialog(this)
        dialog.defaultCloseOperation = JFrame.DISPOSE_ON_CLOSE
        dialog.contentPane = panel
        dialog.pack()
        dialog.setLocationRelativeTo(this)
        dialog.location =
            Point(this.width / 2 - dialog.width / 2, this.height / 2 - dialog.height / 2)
        dialog.isVisible = true
    }
}
