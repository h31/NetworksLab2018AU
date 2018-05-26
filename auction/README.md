# Auction

## Build on Ubuntu

```bash
./gradlew installDist
```

## Run on Ubuntu

### Note

All sample bash commands are expected to be run from `auction` folder.

### Client

```bash
./client -p <port> -h <host> -r <role: in ['participant', 'admin'], 'participant' by default>
```

Examples:

- admin: `./client -p 40000 -h localhost -r admin`

- participant: `./client -p 40000 -h localhost -r participant`


### Server

```bash
./server -p <port> -h <host>
```
