/* Returns the Longest Common Strand from a list of files named "sample.1", "sample.2", etc. */
#define uchar unsigned char
#define min(a,b) ((a)<(b)?(a):(b))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Global Variables, Useful for qsort functions.
uchar* TOTAL_STRING; // concatString in main
int SUM_OF_LENS; // sumOflens in main

/* Helper struct which contains all the info from the files. */
typedef struct {
    // Array of all the different bytestrings
    uchar **strs;
    // Array of lengths of each char array
    int *lens;
} FileInfo;

int lcp(char *a, char *b);
int pstrcmp(const void *a, const void *b);
FileInfo *readFiles(int numFiles);
int findInterval(int num, int *intervalStarts, int numIntervals);
 
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

/* Compares uchar arrays given pointers to them. */
int pstrcmp(const void *a, const void *b) {
    int len1 = SUM_OF_LENS - (*(uchar **)a - TOTAL_STRING);
    int len2 = SUM_OF_LENS - (*(uchar **)b - TOTAL_STRING);
    int cmp = memcmp((uchar *) *(uchar **)a, (uchar *) *(uchar **)b, min(len1, len2));
    if (cmp == 0) {
        if (len2 > len1) { return 1; }
        else if (len1 > len2) { return -1; }
        else { return 0; }
    }
    return cmp;
}

/* Reads a number of output files and returns an array of chars reproduced from the files. */
FileInfo *readFiles(int numFiles) {
    char buffer[FILENAME_MAX];
    uchar **strs = (uchar**)malloc(numFiles*sizeof(uchar*));
    int *lens = (int *)malloc(numFiles*sizeof(int));
    for (int i = 0; i < numFiles; ++i) {
        sprintf(buffer, "sample.%d", i + 1); //Files are 1-indexed...
        FILE* file = fopen(buffer, "rb");

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        uchar* currStr = (uchar*)malloc(size*sizeof(uchar));
        fread(currStr, sizeof(uchar), size, file);

        lens[i] = size;
        strs[i] = currStr;
        fclose(file);
    }
    FileInfo *info;
    info = malloc(sizeof(FileInfo));
    info->strs = strs;
    info->lens = lens;
    return info;
}

/* Finds which numbered interval a given number is in.*/
int findInterval(int num, int *intervalStarts, int numIntervals) {
    int i = 0;
    while(i < numIntervals && num >= intervalStarts[i]) {
        i++;
    }
    return --i;
}

int main() {
    int NUM_FILES;
    printf("How many files would you like to scan? \n");
    scanf("%d", &NUM_FILES);
    if (NUM_FILES < 2) {
        printf("ERROR: Invalid input. \n");
        return -1;
    }
    FileInfo *info = readFiles(NUM_FILES);
    unsigned char **strs = info->strs;
    int *lens = info->lens;
    //Contains the index of the start of each string in the concatenated string.
    int *starts = (int*)malloc(NUM_FILES*sizeof(int));
    int sumOflens = 0;
    for (int i = 0; i < NUM_FILES; ++i) {
        starts[i] = sumOflens;
        sumOflens += lens[i];
    }
    uchar **arrayPointers = (uchar**)malloc((sumOflens)*sizeof(uchar*));
    uchar *concatString = (uchar*)malloc((sumOflens)*sizeof(uchar));

    for (int i = 0; i < NUM_FILES; ++i) {
        memcpy(concatString + starts[i], strs[i], lens[i]);
    }

    // Set up array pointers (representing suffixes)
    for (int i = 0; i < sumOflens; ++i) {
        arrayPointers[i] = &(concatString[i]);
    }

    //Store these things in global vars for use qsort functions
    TOTAL_STRING = concatString;
    SUM_OF_LENS = sumOflens;
    //Sort the suffix array
    qsort(arrayPointers, sumOflens, sizeof(uchar **), pstrcmp);
    
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
        printf("Longest Common Strand found: Length %d \n File 1: sample.%d\t Strand Offset: %d \
                \n File 2: sample.%d\t Strand Offset: %d \n",
               lcslen, lcsstr1, lcssufpos1, lcsstr2, lcssufpos2);
    }
    free(strs);
    free(starts);
    free(arrayPointers);
    free(concatString);
    free(info);
}