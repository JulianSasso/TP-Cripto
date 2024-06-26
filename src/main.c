#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include "include/utils.h"
#include "include/shadowgen.h"
#include "include/bmp.h"
#include "include/steganography.h"
#include "include/reconstruct.h"

/*
 Parametros:

 - argv[1]: d o r
 - argv[2]: imagenSecreta
 - argv[3]: k
 - argv[4]: directorio

*/

int main(int argc, char *argv[]) {
    srand(time(0));

    char * filename, * dirname;
    DIR* dir;
    FILE* file; 
    int k, n = 0;

    if(argc != 5){
        printf("Error: Wrong number of arguments. Should be 4.\n");
        return EXIT_FAILURE;
    }

    k = atoi(argv[3]);
    if(k < MIN_K || k > MAX_K){
        printf("Error: k must be between 3 and 8\n");
        return EXIT_FAILURE;
    }

    if(strcmp("bmp", getExtension(argv[2])) != 0){
        printf("Error: File must have .bmp extension\n");
        return EXIT_FAILURE;
    }

    filename = argv[2];
    dirname = argv[4];

    dir = opendir(dirname);
    if(dir == NULL){
        printf("Error: Directory does not exist\n");
        return EXIT_FAILURE;
    }

    n = checkFileCount(dir);
    if(n < k){
        printf("Error: Directory must contain at least k = %d .bmp images (contains %d)\n", k, n);
        return EXIT_FAILURE;
    }
    closedir(dir);

    if(strcmp(argv[1], "d") == 0){
        file = fopen(filename, "r");
        if(file == NULL){
            printf("Error: Image file does not exist\n");
            return EXIT_FAILURE;
        }
        if(hideSecret(dirname, file, n, k) == EXIT_FAILURE){
            printf("Error: Could not hide the secret\n");
            fclose(file);
            return EXIT_FAILURE;
        }
        fclose(file);
    } else if (strcmp(argv[1], "r") == 0){
        if(reconstruct(filename, dirname, k) == EXIT_FAILURE){
            printf("Error: Could not recover secret\n");
            return EXIT_FAILURE;
        }
    } else {
        printf("Error: First argument must be either 'd' or 'r'");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}