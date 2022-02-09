#pragma once

#include <stdio.h>
#include <stdlib.h>

void die(char * text) {
    perror(text);
    exit(EXIT_FAILURE);
}
