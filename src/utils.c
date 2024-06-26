#include "include/utils.h"

const char *getExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename)
        return "";
    return dot + 1;
}

int checkFileCount(DIR * dir){
    struct dirent * entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_REG){
            if(strcmp("bmp", getExtension(entry->d_name)) != 0 ){
                return -1;
            }
            count++;
        }
    }
    
    return count;   
}

char * getFullPath(const char *dirName, char *entryName){
    int dirNameLen = strlen(dirName);
    char *fullPath = calloc(dirNameLen + strlen(entryName) + 2, sizeof(char));
    if(fullPath != NULL){
        strcat(fullPath, dirName);
        if(fullPath[dirNameLen-1] != '/'){
            strcat(fullPath, "/");
        }
        strcat(fullPath, entryName);
    }
    
    return fullPath;
}

uint8_t ** allocateMatrix(int rows, int cols){
    uint8_t ** m = calloc(rows, sizeof(uint8_t *));
    if(m != NULL){
        for (int i = 0; i < rows; i++){
            m[i] = calloc(cols, sizeof(uint8_t));
            if(m[i] == NULL)
                freeMatrix(m, i);
        }
    }
    return m;
}

void freeMatrix(uint8_t ** m, int rows){
    for (int i = 0; i < rows; i++){
        free(m[i]);
    }
    free(m);
}

uint8_t positiveMod(int n){
    // Multiplying by -(GROUP_MOD - 1) is equivalent to multiplying by 1
    int aux = n;
    if(aux < 0)
        aux *= (-1)*(GROUP_MOD-1); //-250
    return aux % GROUP_MOD;
}

