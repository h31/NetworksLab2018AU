package client.listeners

import client.ClientWindow
import network.http.HttpResponse
import network.requests.AccountsHttpRequest
import java.awt.event.ActionEvent
import javax.swing.JTable
import javax.swing.table.AbstractTableModel
import javax.swing.table.TableModel

class AccountsButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        val request = AccountsHttpRequest()
        executeRequestInsideClientWindow(request) { response ->
            val tableModel = buildModelFromResponse(response)
            val table = JTable(tableModel)
            clientWindow.presentJTable(table)
        }
    }

    companion object {
        private fun buildModelFromResponse(response: HttpResponse): TableModel {
            val accounts = response.body.getJSONArray("accounts")
            return object : AbstractTableModel() {
                override fun getRowCount(): Int = accounts.length()
                override fun getColumnCount(): Int = 1
                override fun getColumnName(column: Int): String = when (column) {
                    0 -> "AccountId"
                    else -> ""
                }
                override fun getValueAt(rowIndex: Int, columnIndex: Int): Any {
                    val account = accounts.getJSONObject(rowIndex)
                    return when (columnIndex) {
                        0 -> account.getInt("id")
                        else -> ""
                    }
                }
            }
        }
    }
}
