package network.http

import com.google.common.base.Preconditions
import com.google.common.io.ByteStreams
import com.google.common.primitives.Bytes
import network.HTTP_ENCODING
import network.HTTP_SEPARATOR
import network.HTTP_VERSION
import org.json.JSONObject
import java.io.InputStream
import java.io.OutputStream
import java.util.*

class HttpResponse(
    val statusCode: Int,
    val statusMessage: String,
    headers: Map<String, String>,
    val body: JSONObject
) : HttpEntity(headers) {

    override fun toString(): String =
        "HttpResponse(" +
            "statusCode=$statusCode, statusMessage='$statusMessage', headers=$headers, body=$body)"

    override fun writeToOutputStream(outputStream: OutputStream) {
        outputStream.write("$HTTP_VERSION $statusCode $statusMessage".toByteArray(HTTP_ENCODING))
        outputStream.write(HTTP_SEPARATOR)
        writeHeadersToOutputStream(outputStream)
        outputStream.write(body.toString().toByteArray(HTTP_ENCODING))
    }

    companion object {
        const val OK_STATUS_CODE = 200
        const val BAD_REQUEST_STATUS_CODE = 400

        fun okFromBody(body: JSONObject = JSONObject()): HttpResponse =
                HttpResponse(
                        HttpResponse.OK_STATUS_CODE,
                        "OK",
                        emptyMap(),
                        body
                )

        fun parseFromInputStream(inputStream: InputStream): HttpResponse {
            val bytes = ByteStreams.toByteArray(inputStream)
            val statusLineEnd = Bytes.indexOf(bytes, HTTP_SEPARATOR)
            Preconditions.checkArgument(statusLineEnd != -1)
            val statusLineBytes = Arrays.copyOf(bytes, statusLineEnd)
            val statusLine = String(statusLineBytes, HTTP_ENCODING)
            val versionEndIndex = statusLine.indexOf(' ')
            Preconditions.checkArgument(versionEndIndex != -1)
            Preconditions.checkArgument(statusLine.substring(0, versionEndIndex) == HTTP_VERSION)
            val statusCodeEndIndex = statusLine.indexOf(' ', versionEndIndex + 1)
            Preconditions.checkArgument(statusCodeEndIndex != -1)
            val statusCode = statusLine.substring(versionEndIndex + 1, statusCodeEndIndex).toInt()
            val statusMessage = statusLine.substring(statusCodeEndIndex + 1)

            val headersStartIndex = statusLineEnd + HTTP_SEPARATOR.size
            val bytesLeft = bytes.sliceArray(headersStartIndex until bytes.size)
            val (headers, endIndex) = parseHeadersFromBytes(bytesLeft)
            val bodyBytes = bytesLeft.sliceArray(endIndex until bytesLeft.size)
            val body = JSONObject(String(bodyBytes, HTTP_ENCODING))
            return HttpResponse(statusCode, statusMessage, headers, body)
        }
    }
}
