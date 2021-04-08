/* Returns the Longest Common Strand from a list of files named "sample.1", "sample.2", etc. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compares strings given pointers to them. */
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
        sprintf(buffer, "sample.%d", i + 1); //Files are 1-indexed...
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
int findInterval(int num, int *intervalStarts, int numIntervals) {
    int i = 0;
    while(i < numIntervals && num >= intervalStarts[i]) {
        i++;
    }
    return --i;
}

int main(int argc, char **argv) {
    if (argc < 0) {
        return -1;
    }
    int NUM_FILES = atoi(argv[1]);
    printf("%d", NUM_FILES);
    if (NUM_FILES < 2) {
        printf("ERROR: Not enough files to find longest strand. \n");
        return -1;
    }
    char **strs = readFiles(NUM_FILES);
    //Contains the index of the start of each string in the concatenated string.
    int *starts = (int*)malloc(NUM_FILES*sizeof(int));
    int sumOflens = 0;
    for (int i = 0; i < NUM_FILES; ++i) {
        starts[i] = sumOflens;
        sumOflens += strlen(strs[i]);
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

    //Sort the suffix array
    qsort(arrayPointers, sumOflens, sizeof(char *), pstrcmp);

    // Length of longest common substring.
    int lcslen = 0;
    // Which numbered strings are the ones which share the lcs.
    int lcsstr1 = -1, lcsstr2 = -1;
    // The positions of the lcs in these strings.
    int lcssufpos1 = -1, lcssufpos2 = -1;
    for (int i = 0; i < sumOflens - 1; ++i) {
        //Figure out where in the len array the adjacent pointers lie.
        //If they lie in the same string, then no need to consider
        int pos1 = arrayPointers[i] - concatString;
        int inter1 = findInterval(pos1, starts, NUM_FILES);
        int pos2 = arrayPointers[i+1] - concatString;
        int inter2 = findInterval(pos2, starts, NUM_FILES);
        if (inter1 != inter2) {
            //If the two suffixes in the suffixes array live in different strings,
            //then their least common prefix is a substring of them both.
            int lcplen = lcp(arrayPointers[i], arrayPointers[i+1]);
            if (lcplen > lcslen) {
                lcslen = lcplen;
                lcsstr1 = inter1 + 1; //1-indexed for filenames
                lcsstr2 = inter2 + 1;
                //The position within a string is position in the concatenated string 
                //minus the where the string starts.
                lcssufpos1 = pos1 - starts[inter1];
                lcssufpos2 = pos2 - starts[inter2];
            }
        }
    }

    if (lcsstr1 == -1) {
        printf("No Common Strand.\n");
    } else {
        printf("Longest Common Strand found: \n File 1: sample.%d ; \t Strand Offset: %d \
                \n File 2: sample.%d ; \t Strand Offset: %d",
               lcsstr1, lcssufpos1, lcsstr2, lcssufpos2);
    }

}