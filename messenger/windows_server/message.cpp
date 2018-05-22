#include <string.h>
#include <stdexcept>
#include "message.h"

elegram::message::message(int socket)
    : _socket(socket) {
	memset(_buffer, 0, 256);
}

char* elegram::message::get_login() {
    return _login;
}

char* elegram::message::get_message() {
    return _message;
}

bool elegram::message::has_next() {
    int n = recv(_socket, _size.byte, 4, 0);
    if (n < 0) {
		return false;
    }
    int login_size = _size.value;
    _login = _read_n(login_size);
    n = recv(_socket, _size.byte, 4, 0);
    if (n <= 0) {
        return false;
    }
    int message_size = _size.value;
    try {
        _message = _read_n(message_size);
    } catch (std::runtime_error& e) {
        return false;
    }
    return true;
}

char* elegram::message::_read_n(int count) {
    auto* out = new char[count];
    int n;
    for (int i = 0; i < count / _BUFFER_SIZE + 1; ++i) {
        if (i == count / _BUFFER_SIZE) {
            n = recv(_socket, _buffer, count - i * _BUFFER_SIZE, 0);
        } else {
            n = recv(_socket, _buffer, _BUFFER_SIZE, 0);
        }
        if (n < 0) {
            throw std::runtime_error("ERROR can't read bytes");
        }
        strcpy(out + i * _BUFFER_SIZE, _buffer);
    }
    return out;
}
