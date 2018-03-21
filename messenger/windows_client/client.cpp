#define _WIN32_WINNT 0x501

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

using namespace std;

volatile bool stopped = false;
volatile bool blocked = false;

unsigned long long to_long(char* arr) {
    unsigned long long res = *((unsigned long long*) arr);
    return res;
}

void write_long(char* arr, unsigned long long value) {
    char* ptr = (char*) &value;
    memcpy(arr, ptr, 8);
}

void write_string(char* arr, string& value) {
    for (int i = 0; i < value.length(); i++) {
        arr[i] = value[i];
    }
}

void* report_listener_stopped() {
    if (stopped) {
        return (void*) 0;
    }
    stopped = true;
    cout << "Connection lost" << endl;
    return (void*) 1;
}

int get_seconds(unsigned long long time) {
    return time % 60;
}

int get_minutes(unsigned long long time) {
    return (time / 60) % 60;
}

int get_hours(unsigned long long time) {
    return (time / 3600) % 24;
}

unsigned long long current_time() {
    time_t rawtime = time(NULL);
    tm timeinfo = *localtime(&rawtime);
    unsigned long long time = 0;
    return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
}

string trim(const string& str) {
    if (str.length() == 0) {
        return str;
    }
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void* connection_listener(void *arg) {
    char header[24];
    char login[101];
    char message[1001];
    SOCKET socket_ptr = (SOCKET) arg;
    while (!stopped) {
        if (recv(socket_ptr, header, 24, 0) != 24) {
            return report_listener_stopped();
        }
        unsigned long long time = to_long(&header[0]);
        unsigned long long login_length = to_long(&header[8]);
        unsigned long long message_length = to_long(&header[16]);
        if (recv(socket_ptr, &login[0], login_length, 0) != login_length) {
            return report_listener_stopped();
        }
        if (recv(socket_ptr, &message[0], message_length, 0) != message_length) {
            return report_listener_stopped();
        }
        login[login_length] = 0;
        message[message_length] = 0;
        while (blocked) {
            if (stopped) {
                return report_listener_stopped();
            }
        }
        printf("[%02d:%02d:%02d from %s] %s\n", get_hours(time), get_minutes(time), get_seconds(time), login, message);
    }
    return (void*) 0;
}

void start_communication(SOCKET connection, string login) {
    pthread_t listener;
    int err_code = pthread_create(&listener, NULL, connection_listener, (void*) connection);
    if (err_code < 0) {
        cout << "Error: new thread creation failed. Error code: " << err_code << endl;
        return;
    }
    cout << "Hello, " << login << "!" << endl;

    string message;
    char buffer[1200];
    write_long(&buffer[8], login.length());
    write_string(&buffer[24], login);
    while (!stopped) {
        getline(cin, message);
        message = trim(message);
        if (message == "\\m") {
            blocked = true;
            cout << endl << ">";
            continue;
        }
        if (message.length() > 1000) {
            cout << "Error: message is too long! Max length: 1000" << endl;
            blocked = false;
            continue;
        }
        write_long(&buffer[0], current_time());
        write_long(&buffer[16], message.length());
        write_string(&buffer[24 + login.length()], message);
        unsigned long long total_length = 24 + login.length() + message.length();
        if (send(connection, buffer, total_length, 0) != total_length) {
            if (!stopped) {
                cout << "Error: connection lost" << endl;
                stopped = true;
            }
            break;
        }
        blocked = false;
    }
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Error: wrong arguments formate. Expected: \"host\" \"port\" \"login\"" << endl;
        return 1;
    }
    string login(argv[3]);
    if (login.length() > 100) {
        cout << "Error: login is too long! Max length: 100" << endl;
        return 2;
    }
    WSADATA wsaData;
    int err_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err_code != 0) {
        cout << "Error: WSAStartup failed. Error code: " << err_code << endl;
        return 3;
    }
    struct addrinfo* addr = NULL;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    err_code = getaddrinfo(argv[1], argv[2], &hints, &addr);
    if (err_code != 0) {
        cout << "Error: getaddrinfo failed. Error code: " << err_code << endl;
        WSACleanup();
        return 4;
    }
    SOCKET connection = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (connection == INVALID_SOCKET) {
        cout << "Error: socket creation failed." << endl;
        freeaddrinfo(addr);
        WSACleanup();
        return 5;
    }
    err_code = connect(connection, addr->ai_addr, addr->ai_addrlen);
    freeaddrinfo(addr);
    if (err_code == SOCKET_ERROR) {
        cout << "Error: unable to connect to server!" << endl;
        closesocket(connection);
        WSACleanup();
        return 6;
    }

    start_communication(connection, login);

    err_code = shutdown(connection, SD_BOTH);
    if (err_code == SOCKET_ERROR) {
        cout << "Error: shutdown failed. Error code: " << err_code << endl;
        closesocket(connection);
        WSACleanup();
        return 7;
    }
    err_code = closesocket(connection);
    if (err_code != 0) {
        cout << "Error: socket closing failed. Error code: " << err_code << endl;
        WSACleanup();
        return 8;
    }
    WSACleanup();
    return 0;
}
