#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct message {
    string message_text;
    string sender;
    time_t time;
};

void handle_error(string s) {
    cerr << s << "\n";
}