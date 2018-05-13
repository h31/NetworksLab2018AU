package ru.spbau.mit.tracker.request;

import ru.spbau.mit.common.RequestConfig;

public class ListRequest implements TrackerRequest {

    @Override
    public byte getType() {
        return RequestConfig.LIST_REQUEST;
    }
}
