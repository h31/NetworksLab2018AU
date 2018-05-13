package ru.spbau.mit.tracker.request;

import ru.spbau.mit.common.RequestConfig;

public class SourcesRequest implements TrackerRequest {
    private int fileId;

    public SourcesRequest(int fileId) {
        this.fileId = fileId;
    }

    public int getFileId() {
        return fileId;
    }

    @Override
    public byte getType() {
        return RequestConfig.SOURCES_REQUEST;
    }
}
