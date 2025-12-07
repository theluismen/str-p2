#include <stdio.h>
#include <stdlib.h>

int main ( int argc, char * argv[] ) {
    int num_hilos;

    if ( argc != 2) {
        return 1;
    }

    num_hilos = atoi(argv[1]);
    printf("num_hilos: %d", num_hilos);

    return 0;
}
