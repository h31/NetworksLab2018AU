# Build

## Ubuntu
```bash
cmake .
make -j $(nproc)
```

## Windows
Can be configured from CMake GUI application and built from Microsoft Visual Studio.

# Usage

## Ubuntu
Running client:
```bash
linux_server/server_linux <port>
```

Running server:
```bash
linux_client/client_linux <hostname> <port> <username>
```

## Windows
Can be run from Microsoft Visual Studio after proper CMake configuration.
