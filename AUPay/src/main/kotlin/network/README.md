Types of requests:

1. Registration:

    ```http request
    POST /register HTTP/1.1
    ```

    ```http request
    HTTP/1.1 200 OK
    Set-Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760

    {"accountId":153}
    ```

2.  Requesting all accounts:

    ```http request
    GET /accounts HTTP/1.1
    ```

    ```http request
    HTTP/1.1 200 OK

    {"accounts":[{"id":153},{"id":289}, {"id":793}]}
    ```

3.  Requesting the account balance associated with the given AuthToken:

    ```http request
    GET /balance HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK

    {"balance":15000}
    ```

4. Asking to place an order of payment:

    ```http request
    POST /pay/amount/toId HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK

    {"paymentId":147982}

5. Asking to place a request of payment:

    ```http request
    POST /request/amount/fromId HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK

    {"paymentId":147982}

6. Get all payments from or to the user associated with the given AuthToken:

    ```http request
    GET /payments
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK

    {"payments":[{"amount":500,"fromAccountId":153,"paymentId":147982,"toAccountId":289,"status":"PLACED"},{"amount":50,"fromAccountId":793,"paymentId":213009,"toAccountId":153,"status":"SUCCESSFUL"}]}
    ```

    PaymentStatus can be one of the following:
    *   PLACED
    *   CONFIRMED
    *   DENIED
    *   CANCELLED
    *   SUCCESSFUL

7. Confirming an incoming payment request:

    ```http request
    POST /confirm/paymentId HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK
    
    {}
    ```

8. Denying an incoming payment request:
    
    ```http request
    POST /deny/paymentId HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK
    
    {}
    ```    
9. Asking to cancel an order of payment:

    ```http request
    POST /cancel/paymentId HTTP/1.1
    Cookie: AuthToken=46c000b6-3717-4de7-a530-193d34e5b760
    ```

    ```http request
    HTTP/1.1 200 OK

    {}
