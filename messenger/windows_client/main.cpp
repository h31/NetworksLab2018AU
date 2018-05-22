#pragma comment(lib, "WS2_32.lib")
#include <cstdlib>
#include "client.h"

char * getline(void) {
    char * line = static_cast<char *>(malloc(100)), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == nullptr)
        return nullptr;
    fgetc(stdin);
    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = static_cast<char *>(realloc(linep, lenmax *= 2));

            if(linen == nullptr) {
                free(linep);
                return nullptr;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        return -1;
    }
    elegram::client c(argv[1], atoi(argv[2]), argv[3]);
    c.connect_to_server();
    do {
        char ch;
        ch = getchar();
        switch (ch) {
            case 'm': {
                printf("Write message:\n");
                c.set_print(false);
                char *l;
                l = getline();
                c.send_message(l);
                free(l);
                c.set_print(true);
                break;
            }
            case 'q': {
                c.stop();
                return 0;
            }

        }
    } while (true);
}