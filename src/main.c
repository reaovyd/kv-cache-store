#include <stdio.h>
#include <event2/event.h>

#include "macro.h"
#include "server.h"

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    server_init();


    return 0;
}
