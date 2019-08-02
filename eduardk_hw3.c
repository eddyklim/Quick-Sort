/*
 * eduardk_hw3.c
 *
 *  Created on: Jan 26, 2018
 *  Author: Eduard Klimenko
 *
 *  This program reads in multiple text files, stores the data into arrays,
 *  and sorts the data alphabetically using quicksort, this is then written
 *  to a csv file. Each line in the files contain a name, gender, and number
 *  of births (for example, Mary,F,70980). Females are listed first and
 *  the files are already sorted by popularity. This lets us ignore the
 *  gender and actual amount of births.
 *
 */

/* Headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAME_LENGTH 21
#define MAX_NAMES 4000
#define RANK_LENGTH 10

/*
 * Functions in this program.
 */
void processLine(char* temp, char names[][NAME_LENGTH],
        int ranks[][RANK_LENGTH], int currentRank, int yearOffset,
        int* uniqueNames);
void processFile(FILE *n, char names[][NAME_LENGTH], int ranks[][RANK_LENGTH],
        int yearOffset, int* uniqueNames);
void processFiles(char names[][NAME_LENGTH], int ranks[][RANK_LENGTH],
        int* uniqueNames);
int findNamePosition(char a[][NAME_LENGTH], char *theName, int numberOfNames);
void quickStrSort(char *left, char *right, int length, char a[][NAME_LENGTH],
        int ranks[][RANK_LENGTH]);
void writeData(char names[][NAME_LENGTH], int ranks[][RANK_LENGTH]);

/*
 * This is the main function. Creates two arrays, one for storing names
 * and the other for storing the ranks. These arrays are parallel (meaning
 * index n of names array will also be the same index in the ranks array for
 * that person). UniqueName is a counter used later to make sure to not add
 * duplicate names.
 */
int main() {
    /* Eclipse compatibility for this program. */
    setvbuf(stdout, NULL, _IONBF, 0);
    char names[MAX_NAMES][NAME_LENGTH];
    int ranks[MAX_NAMES][RANK_LENGTH];
    int uniqueNames = 0;

    processFiles(names, ranks, &uniqueNames);
    quickStrSort(names[0], names[365], NAME_LENGTH, names, ranks);
    writeData(names, ranks);

    return 0;
}

/*
 * This function takes in the names array and ranks array. It then opens a file
 * for every decade starting at 1920.
 */
void processFiles(char names[][NAME_LENGTH], int ranks[][RANK_LENGTH],
        int* uniqueNames) {
    char fName[16];
    int yearOffset = 0;
    for (int z = 1920; z <= 2010; z = z + 10) {
        sprintf(fName, "yob%d.txt", z);
        FILE *infile = fopen(fName, "r");
        processFile(infile, names, ranks, yearOffset, uniqueNames);
        fclose(infile);
        yearOffset++;
    }
}

/*
 * This function takes in the names array and ranks array including an offset and
 * a pointer to the number of unique names. It then calls a function to process a
 * single line 100 times. The offset is used to move along the columns in the ranks
 * array. Since the file is already sorted by popularity we can use a simple counter.
 */
void processFile(FILE *n, char names[][NAME_LENGTH], int ranks[][RANK_LENGTH],
        int yearOffset, int* uniqueNames) {
    int nameRead = 1;
    char temp[100];
    while (nameRead <= 100) {
        fscanf(n, "%s", temp);
        processLine(temp, names, ranks, nameRead, yearOffset, uniqueNames);
        nameRead++;
    }
}

/*
 * This function takes in the names array and ranks array including an offset and
 * a pointer to the number of unique names. It then parses the line by commas. The
 * name is always first so we just grab the first token and skip the next two.
 * Adds a new name and its corresponding rank if the name is unique.
 */
void processLine(char* temp, char names[][NAME_LENGTH],
        int ranks[][RANK_LENGTH], int currentRank, int yearOffset,
        int* uniqueNames) {

    char *token = strtok(temp, ",");
    int numTokens = 0;
    char name[NAME_LENGTH];

    // Parses the line based on commas.
    while (token != NULL) {
        if (numTokens == 0)
            strcpy(name, token);
        token = strtok(NULL, ",");
        numTokens++;
    }

    // Adds to ranks if the name is the same.
    int sameNameCount = 0;
    for (int i = 0; i < MAX_NAMES; i++) {
        if (strcmp(names[i], name) == 0) {
            sameNameCount++;
            ranks[i][yearOffset] = currentRank;
        }
    }

    // Adds a new name to array and its corresponding rank.
    if (sameNameCount == 0) {
        strcpy(names[*uniqueNames], name);
        ranks[*uniqueNames][yearOffset] = currentRank;
        (*uniqueNames)++;
    }
}
/*
 * This function takes in the names array and ranks array and the writes to a
 * cvs file. Prints a name and then non-zero ranks (a comma is included after
 * every name and value for formatting purposes in csv).
 */
void writeData(char names[][NAME_LENGTH], int ranks[][RANK_LENGTH]) {
    FILE *outfile = fopen("rankings.csv", "w+");
    fprintf(outfile, "Name,");
    for (int l = 1920; l <= 2010; l = l + 10) {
        fprintf(outfile, "%d,", l);
    }
    fprintf(outfile, "\n");
    for (int i = 0; i < 366; i++) {
        fprintf(outfile, "%s", names[i]);
        for (int k = 0; k < RANK_LENGTH; k++) {
            fprintf(outfile, ",");
            if (ranks[i][k] != 0)
                fprintf(outfile, "%d", ranks[i][k]);
        }
        fprintf(outfile, "\n");
    }
    fclose(outfile);
}

/*
 * This function takes in the names array and ranks array including the left
 * and right pointers for the partition. The algorithm is used to sort the
 * names alphabetically but since the ranks are linked linearly we must also
 * swap the rows in the ranks array.
 */
void quickStrSort(char *left, char *right, int length,
        char names[][NAME_LENGTH], int ranks[][RANK_LENGTH]) {

    char *i = left;
    char *j = right;
    char swap[21];
    char test[21];
    int nswap[10];
    char *s2 = j;

    strncpy(test, s2, length);

    do {
        while (strcmp(i, test) < 0)
            i += length;
        while (strcmp(j, test) > 0)
            j -= length;

        if (i <= j) {
            // Swaps the names.
            strcpy(swap, i);
            strcpy(i, j);
            strcpy(j, swap);

            int n1 = findNamePosition(names, i, 366);
            int n2 = findNamePosition(names, j, 366);

            // Swaps the ranks also.
            memcpy(nswap, ranks[n1], 40);
            memcpy(ranks[n1], ranks[n2], 40);
            memcpy(ranks[n2], nswap, 40);

            i += length;
            j -= length;
        }

    } while (i <= j);
    if (left < j)
        quickStrSort(left, j, length, names, ranks);
    if (i < right)
        quickStrSort(i, right, length, names, ranks);
}

/*
 * This function takes in the names array and a specific name. The return
 * value is the row in which the sits.
 */
int findNamePosition(char arr[][NAME_LENGTH], char *curName, int numNames) {
    int result = 0;
    for (int i = 0; i < numNames; i++) {
        if (strcmp(*(arr + i), curName) == 0)
            result = i;
    }
    return result;
}
