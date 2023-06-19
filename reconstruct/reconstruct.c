#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../bmp/bmp.h"
#include "../utils/utils.h"

void reconstruct(char * outputName, char * sourceDirName, int k);
void recoverShadow(FILE * participant, int k, uint8_t * shadow, long shadowLen);
static uint8_t ** allocateMatrix(int rows, long cols);
static void freeMatrix(uint8_t ** m, long rows);

int main(int argc, char *argv[]){
    printf("%s\n\n", argv[1]);
    reconstruct("", argv[1], 3);
    return 0;
}


void reconstruct(char * outputName, char * sourceDirName, int k){
    DIR* dir = opendir(sourceDirName);
    if(dir == NULL){
        printf("malardo\n");
        return;
    }
    struct dirent * entry;

    int dirNameLen = strlen(sourceDirName);

    
    char * fullPath;
    FILE * participant;
    int processed = 0;

    long width, height, t, shadowLen;
    uint8_t ** shadows = NULL;
    uint8_t * preimages = malloc(sizeof(uint8_t) * k);

    while(processed < k && ((entry = readdir(dir)) != NULL)){
        
        if (entry->d_type == DT_REG) {

            fullPath = getFullPath(sourceDirName, dirNameLen, entry->d_name);
            
            participant = fopen(fullPath, "r");
            if(participant == NULL){
                printf("Malardo 2\n");
                return;
            }
            
            if(shadows == NULL){
                readHeaderSetOffet(participant, &width, &height);
                t = (width*height) / (2*k - 2);
                shadowLen = 2*t;
                shadows = allocateMatrix(k, shadowLen);
            }

            BITMAPFILEHEADER * participantHeader = ReadBMFileHeader(participant);
            preimages[processed] = participantHeader->bfReserved1;
            recoverShadow(participant, k, shadows[processed], shadowLen);

            //recoverShadow()
            processed++;
            fclose(participant);
        }
        
    }
    if (processed < k){
        // TODO: Salida con error
    }

    uint8_t ** vm = allocateMatrix(t, k);
    uint8_t ** vd = allocateMatrix(t, k);
    // TODO: A chequear
    for (int j = 0; j < k; j++) {
        for (int i = 0; i < shadowLen; i += 2) {
            vm[i/2][j] = shadows[j][i];
            vd[i/2][j] = shadows[j][i+1];
        }
    }
    
    closedir(dir);
    freeMatrix(shadows, shadowLen);
}

void recoverShadow(FILE * participant, int k, uint8_t * shadow, long shadowLen){

    // Para lsb2, leo de a 4
    // Para lsb4, leo de a 2
    uint8_t lsbValue, lsbMask, bytesToRead;

    if(k <= 4){
        lsbValue = 4;
        lsbMask = 0x0F; // 0000 1111
        bytesToRead = 2; 
    } else {
        lsbValue = 2;
        lsbMask = 0x03; // 0000 0011
        bytesToRead = 4;
    }

    uint8_t * buf = calloc(bytesToRead, sizeof(uint8_t));
    for (long i = 0; i < shadowLen; i++){
        uint8_t shadowByte = 0x00;

        if(fread(buf, sizeof(uint8_t), bytesToRead, participant) < 8/lsbValue){
            // TODO: Manejar error
            printf("Me falto leer \n");
            return;
        }

        for (int j = 0; j < bytesToRead; j++){
            shadowByte <<= lsbValue;
            shadowByte += buf[j] & lsbMask;
        }
        
        shadow[i] = shadowByte;
    }
}


static uint8_t ** allocateMatrix(int rows, long cols){
    uint8_t ** m = calloc(rows, sizeof(uint8_t *));
    for (long i = 0; i < rows; i++){
        m[i] = calloc(cols, sizeof(uint8_t));
    }
    return m;
}

static void freeMatrix(uint8_t ** m, long rows){
    for (int i = 0; i < rows; i++){
        free(m[i]);
    }
    free(m);
}