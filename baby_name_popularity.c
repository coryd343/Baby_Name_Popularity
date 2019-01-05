/*
* HW 3: Baby Names
* Author: Cory Davis
* Version: 4/19/17
*/

#include <stdio.h>
#include <string.h>

#define NAMELEN 20
#define NUMYEARS 10
#define NUMRECORDS 1000

int isDuplicate(char newName[], char allNames[][NAMELEN], int *size);
int namePosition(char newName[], char allNames[][NAMELEN], int *size);
void collectData(char *files[], char names[][NAMELEN], int ranks[][NUMYEARS], int *size);
void readData(char names[][NAMELEN], int ranks[][NUMYEARS], int *size);
void swapRows(int row1, int row2, char names[][NAMELEN], int ranks[][NUMYEARS]);
void sortData(char names[][NAMELEN], int ranks[][NUMYEARS], int first, int last);
void writeFile(FILE *out, char names[][NAMELEN], int ranks[][NUMYEARS], int size);

int main(void) {
    FILE *output = fopen("summary.csv", "w");
    FILE *currFile;
    char *fileNames[] = {"names/yob1920.txt", "names/yob1930.txt", "names/yob1940.txt", "names/yob1950.txt", "names/yob1960.txt", "names/yob1970.txt", "names/yob1980.txt", "names/yob1990.txt", "names/yob2000.txt", "names/yob2010.txt"};
    char names[NUMRECORDS][NAMELEN];
    int size = 0;
    int ranks[NUMRECORDS][NUMYEARS];
    collectData(fileNames, names, ranks, &size);
    readData(names, ranks, &size);
    //readData(names, ranks, &size);
    sortData(names, ranks, 0, size - 1);
    readData(names, ranks, &size);
    //Last record: row 366, column 9
    writeFile(output, names, ranks, size);
    fclose(output);
}

//returns the position of the duplicate if the given name exists in the array already and -1 if it does not already exist
int isDuplicate(char newName[], char allNames[][NAMELEN], int *size) {
    for (int i = 0; i < *size; i++) {
        //print("%d. Comparing %s to %s\n", i, newName, allNames[i]);
        if (!strcmp(newName, allNames[i])) {
            printf("Duplicate: %s is at position %d: %s\n", newName, i, allNames[i]);
            return i; //is a duplicate
        }
    }
    printf("New\n");
    return -1; //not a duplicate
}

//Returns the position of a name in the array of names
int namePosition(char newName[], char allNames[][NAMELEN], int *size) {
    for (int i = 0; i < *size; i++) {
        if (!strcmp(newName, allNames[i])) {
            return i;
        }
    }
    return -1;
}

//Reads in data from each of the text files into the arrays for names and ranks
void collectData(char *files[], char names[][NAMELEN], int ranks[][NUMYEARS], int *size) {
    FILE *currFile = fopen(files[0], "r");
    char currName[NAMELEN];
    int currRank, duplicatePosition;
    fscanf(currFile, " %[^,],F,%d", &currName, &currRank);
    strcpy(names[*size], currName);
    ranks[*size][0] = currRank;
    ++(*size);
    for(int i = 1; i < NUMRECORDS + 1; i++) {
        printf("Loop: %d\n", i);
        if(i % 100 == 0 && i != NUMRECORDS) { //If we've read 100 records, change to the next file
            fclose(currFile);
            printf("\n********\nOpening file: %s\n********\n\n", files[i / 100]);
            currFile = fopen(files[i / 100], "r");
        }
        fscanf(currFile, " %[^,],F,%d", &currName, &currRank);
        printf("Name: %s, Rank: %d\n", currName, currRank);
        duplicatePosition = isDuplicate(currName, names, size);
        if(duplicatePosition == -1) { //If this is the first time this name is encountered
            strcpy(names[*size], currName);
            if(i == 1000) {
                ranks[*size][9] = currRank;
            }
            else {
                ranks[*size][i / 100] = currRank;
            }
            ++(*size);
        }
        else { //This name has been encountered before
            if(i == 1000) {
                ranks[duplicatePosition][9] = currRank;
            }
            else {
                ranks[duplicatePosition][i / 100] = currRank;
            }
        }
    }
}

//Prints the data from both arrays containing the names and ranks
void readData(char names[][NAMELEN], int ranks[][NUMYEARS], int *size) {
    printf("\n\nPrinting!\n");
    for(int i = 0; i < *size; i++) {
        printf("%-3d. %11s: ", i + 1, names[i]);
        for(int j = 0; j < NUMYEARS; j++) {
            printf(" %6d", ranks[i][j]);
        }
        printf("\n");
    }
}

//Swaps two rows in both the name array and the rank array
void swapRows(int row1, int row2, char names[][NAMELEN], int ranks[][NUMYEARS]) {
    char tempName[NAMELEN];
    int tempRank;
    strcpy(tempName, names[row1]); //Put row 1 in the temp variable
    strcpy(names[row1],names[row2]); //Overwrite row 1 with row 2
    strcpy(names[row2], tempName); //Overwrite row 2 with the temp variable (containing row 1's data)
    
    for(int i = 0; i < NUMYEARS; i++) {
        tempRank = ranks[row1][i];
        ranks[row1][i] = ranks[row2][i];
        ranks[row2][i] = tempRank;
    }
}

//Sorts the names in the first array alphabetically and moves the ranks in parallel
void sortData(char names[][NAMELEN], int ranks[][NUMYEARS], int first, int last) {
    //printf("Now sorting!\n");
    //printf("First: %d, Last: %d\n", first, last);
    if(last - first < 2) {
        //printf("***Returning***\n");
        return;
    }
    int i = first, j = first;
    char pivot[NAMELEN];
    strcpy(pivot, names[last]);
    while(j < last) {
        if(strcmp(pivot, names[j]) > 0) {
            swapRows(i, j, names, ranks);
            i++;
        }
        j++;
    }
    swapRows(i, last, names, ranks);
    sortData(names, ranks, first, i - 1);
    sortData(names, ranks, i + 1, last);
}

//Writes the data from the name and rank array into a csv file
void writeFile(FILE *out, char names[][NAMELEN], int ranks[][NUMYEARS], int size) {
    fprintf(out, "Name");
    for(int i = 0, year = 1920; i < NUMYEARS; i++) {
        fprintf(out, ",%d", year);
        year += 10;
    }
    fprintf(out, "\n");
    for(int i = 0; i < size; i++) {
        fprintf(out, "%s", names[i]);
        for(int j = 0; j < NUMYEARS; j++) {
            fprintf(out, ",%d", ranks[i][j]);
        }
        fprintf(out, "\n");
    }
}
