#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper that compares the points behind the strings. */
int pstrcmp(const void *a, const void *b) {
    return strcmp((const char *)*(char **)a, (const char *)*(char **)b);
}

 
/* Returns the longest common prefix of two strings. */
int lcp(char *a, char *b) {
    int len1 = strlen(a);
    int len2 = strlen(b);
    int i;
    for (i = 0; (i < len1) && (i < len2); ++i) {
        if (a[i] != b[i]) break;
    }
    return i;
}

/* Reads a number of output files and returns an array of strings reproduced from the files. */
char **readFiles(int numFiles) {
    char buffer[FILENAME_MAX];
    char **strs = (char**)malloc(numFiles*sizeof(char*));
    
    for (int i = 0; i < numFiles; ++i) {
        sprintf(buffer, "sample.%d", i);
        FILE* file = fopen(buffer, "r");

        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* currStr = (char*)malloc(size*sizeof(char) + 1);
        fread(currStr, sizeof(char), size, file);
        currStr[size] = 0;

        strs[i] = currStr;
        fclose(file);
        free(currStr);
    }
    return strs;
}

/* Finds which numbered interval a given number is in.*/
int findInterval(int num, int *intervalLens) {
    int sofarLen = 0;
    int i = 0;
    while(num >= sofarLen) {
        sofarLen += intervalLens[i++];
    }
    return --i;
}

int main(int argc, char **argv) {
    int NUM_FILES = atoi(argv[1]);
    printf("%d", NUM_FILES);
    if (NUM_FILES < 2) {
        printf("ERROR: Not enough files to find longest strand. \n");
        return -1;
    }
    char **strs = readFiles(NUM_FILES);

    int *lens = (int*)malloc(NUM_FILES*sizeof(int));
    int sumOflens = 0;
    for (int i = 0; i < NUM_FILES; ++i) {
        lens[i] = strlen(strs[i]);
        sumOflens += lens[i];
    }

    char **arrayPointers = (char**)malloc((sumOflens)*sizeof(char*));
    char *concatString = (char*)malloc((sumOflens)*sizeof(char));

    concatString[0] = 0;
    for (int i = 0; i < NUM_FILES; ++i) {
        strcat(concatString, strs[i]);

    }

    // Set up array pointers (representing suffixes)
    for (int i = 0; i < sumOflens; ++i) {
        arrayPointers[i] = &(concatString[i]);
    }

    for (int i = 0; i < sumOflens; ++i) {
        printf("%s\n", arrayPointers[i]);
    }

    qsort(arrayPointers, sumOflens, sizeof(char *), pstrcmp);

    for (int i = 0; i < sumOflens; ++i) {
        printf("%s\n", arrayPointers[i]);
    }

    printf("finding the lcsn");

    int lcslen = 0, lcplen, lcssufpos = -1;
    for (int i = 0; i < sumOflens - 1; ++i) {

        //figure out where in the len array the adjacent pointers lie.
        // If they lie in the same string, then no need to consider
        int inter1 = findInterval(arrayPointers[i] - concatString, lens);
        int inter2 = findInterval(arrayPointers[i+1] - concatString, lens);
        if (inter1 != inter2) {
            lcplen = lcp(arrayPointers[i], arrayPointers[i+1]);
            if (lcplen > lcslen) {
                lcslen = lcplen;
                lcssufpos = i;
            }
        }
    }

    if (lcssufpos == -1) {
        printf("no lcs\n");
    } else {
        printf("%.*s\n", lcslen, arrayPointers[lcssufpos]);
    }

}