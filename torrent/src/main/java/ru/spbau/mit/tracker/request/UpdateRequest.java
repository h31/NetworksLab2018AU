package ru.spbau.mit.tracker.request;

import ru.spbau.mit.common.RequestConfig;
import ru.spbau.mit.data.ClientDataInfo;


public class UpdateRequest implements TrackerRequest {
    private ClientDataInfo clientDataInfo;

    public UpdateRequest(ClientDataInfo clientDataInfo) {
        this.clientDataInfo = clientDataInfo;
    }

    public ClientDataInfo getClientDataInfo() {
        return clientDataInfo;
    }

    @Override
    public byte getType() {
        return RequestConfig.UPDATE_REQUEST;
    }
}
