# Elegram

Simple one-room messenger with server and client

## Server 

CMake target `server`.

Is run by issuing `./server port` command with appropriate port substituted.

Listens on given port, accepts incoming connections and broadcasts incoming messages to logged in clients.

## Client

CMake target `client`.

Is run by issuing `./client server_addr port nickname` command with appropriate values substituted. Nickname should not be empty.

Connects to server, logs in to server and communicates with server to send and receive message to/from other clients.

Commands:
- `:q` logs out and quits client
- `:m` message input mode -- reads a line and sends to server. New messages are not being printed while in this mode

## Communication protocol between server and clients

Clients can send several types of packets to server:
- login packet   -- [LOGIN, nickname size in characters + 1 for `\0`, nickname]
- message packet -- [MESSAGE, message size in characters + 1 for `\0`, message]
- logout packet  -- [TYPE]

LOGIN, MESSAGE, TYPE are instances of enumeration class, which size is architecture-defined.

Sizes are represented by `size_t` instances.

Nickname and message are `\0`-terminated `char` arrays.

Client can login only once and any messages from it are ignored before logging in.
After client logout server will ignore any incoming messages from it and close the socket as soon as possible.

Server sends a single type of packet to clients representing messages:
- server packet -- [server received time, sender nickname size in characters + 1 for `\0`, sender nickname, message size in characters + 1 for `\0`, message]

Server received time is UTC and is represented by a string in ISO 8061 format (`2018-03-16T16:49:25Z`).

Strings and its sizes are the same way as in client messages.