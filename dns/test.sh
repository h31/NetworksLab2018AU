#!/bin/bash -eux

function clean {
    kill ${SERVER_PID}
}

function error {
    clean
    exit 1
}

PORT=8080
./build/server/audns_server ${PORT} &
SERVER_PID=$!

# actual tests begin
# =====================

(sleep 3 && pkill audns_client) &
./build/client/audns_client 8.8.8.8 example.com || error
sleep 4

(sleep 3 && pkill audns_client) &
./build/client/audns_client 127.0.0.1 example.com "${PORT}" || error
sleep 4

dig example.com @127.0.0.1 -p "${PORT}" +time=1 +tries=1 || error

# =====================
# actual tests end

clean
