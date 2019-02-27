#include <stdio.h>

#include <stdlib.h>

int main(void){

    int i;
    i = execl("/bin/ping", "ping", "127.0.0.1");
    printf("%d ", i);
    return 0;
}
