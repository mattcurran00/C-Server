#include "getDirectory.h"
#include <string.h>

char *getDir(char *path){

    char *extension = strrchr(path, '.');

    if (extension == NULL){
        return NULL;
    }
    if (strcmp(extension, ".html") == 0){
        return "html";
    }
    if (strcmp(extension, ".js") == 0) {
        return "js";
    }

    return NULL;
}