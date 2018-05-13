package ru.spbau.mit.client.request;

import ru.spbau.mit.common.RequestConfig;
import ru.spbau.mit.tracker.request.TrackerRequest;

import java.io.Serializable;

public class StatClientRequest implements TrackerRequest, Serializable {
    private int fileId;

    public StatClientRequest(int fileId) {
        this.fileId = fileId;
    }

    @Override
    public byte getType() {
        return RequestConfig.STAT_REQUEST;
    }

    public int getFileId() {
        return fileId;
    }
}
