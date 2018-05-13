package ru.spbau.mit.client.request;

import ru.spbau.mit.common.RequestConfig;
import ru.spbau.mit.tracker.request.TrackerRequest;

import java.io.Serializable;

public class GetClientRequest implements TrackerRequest, Serializable {
    private int fileId;
    private int filePart;

    public GetClientRequest(int fileId, int filePart) {
        this.fileId = fileId;
        this.filePart = filePart;
    }

    @Override
    public byte getType() {
        return RequestConfig.GET_REQUEST;
    }

    public int getFileId() {
        return fileId;
    }

    public int getFilePart() {
        return filePart;
    }
}
