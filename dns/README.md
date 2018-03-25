# Elegram

Simple DNS server and client supporting just A-type records

## Server 

CMake target `server`.

Is run by issuing `./server` command.

Listens on port 53, gets client requests and tries to find IPs to hosts they search for.

Server listens for various CLI termination signals and tries to graciously stop upon getting any of them.

## Client

CMake target `client`.

Is run by issuing `./client serveraddr hostname` command with appropriate values substituted.

Sends DNS request for IPv4 of hostname to the specified server and shows the answer.
