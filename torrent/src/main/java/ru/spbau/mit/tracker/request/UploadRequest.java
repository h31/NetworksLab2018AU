package ru.spbau.mit.tracker.request;

import ru.spbau.mit.common.RequestConfig;

public class UploadRequest implements TrackerRequest {
    private String name;
    private long size;

    public UploadRequest(String name, long size) {
        this.name = name;
        this.size = size;
    }

    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    @Override
    public byte getType() {
        return RequestConfig.UPLOAD_REQUEST;
    }
}
