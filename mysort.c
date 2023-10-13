#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_LINE_LEN 4096
#define MIN_NLINES 100

typedef int (*cmp_fn_t)(const void *p, const void *q);

//compares void*'s as strings and compares them, returning the result of comparison
int cmp_pstr(const void *p, const void *q) {
    //defererence and cast void *'s to char * variables
    char *str1 = *(char **)p;
    char *str2 = *(char **)q;

    //return the comparison of the two strings
    return strcmp(str1, str2);
}

//compares void *'s as strings and returns result of length-based comparison
int cmp_pstr_len(const void *p, const void *q) {
    //dereference and cast void *'s
    char *str1 = *(char **)p;
    char *str2 = *(char **)q;

    //store the length of the strings
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    //find the longer line and set val accordingly
    //the else statement is -1 if they are not equal and 0 if they are, just a little boolean algebra
    int val = (len1 > len2) ? 1 : -1 * !(len1 == len2);

    return val;
    
    
}

//compares void *'s as strings and returns result of int value comparison
int cmp_pstr_numeric(const void *p, const void *q) {
    //dereference and cast void *'s
    char *str1 = *(char **)p;
    char *str2 = *(char **)q;

    //store the int value of the strings as determined by atoi
    int num1 = atoi(str1);
    int num2 = atoi(str2);
    
    //else statement uses bool algebra to return -1 if != and 0 if == appropriately
    int val = (num1 > num2) ? 1 : -1 * !(num1 == num2);
    return val;
}

/* sort lines uses stack space, heap memory, and generic function pointers to sort
   appropriately sort the lines of a file based on a variety of cmd line flags*/
void sort_lines(FILE *fp, cmp_fn_t cmp, bool uniq, bool reverse) {
    //stack array to hold a line
    char line[MAX_LINE_LEN];

    char **lines = malloc(MIN_NLINES * sizeof(&line));
    assert(lines != NULL);

    size_t lineCount = 0;
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        //resize lines if we need to
        if (lineCount % MIN_NLINES == 0 && lineCount > 0) {
            lines = realloc(lines, MIN_NLINES * ((lineCount / MIN_NLINES) + 1) * sizeof(&line));
            assert(lines != NULL);
        }

        if (uniq) {
            char *newln = strdup(line);
            binsert(&newln, lines, &lineCount, sizeof(&line), cmp);
        } else {
            lines[lineCount] = strdup(line);
            lineCount++;
        }
        //printf("%d\n", lineCount - 1);
    }

    //sort the array if we didn't get -u
    qsort(lines, lineCount, sizeof(&line), cmp);


    //print out the file in the appropriate order based on cmd line flags
    for (int i = 0; i < lineCount; i++) {
        if (reverse) {
            printf("%s", lines[lineCount - 1 - i]);
            free(lines[lineCount - 1 - i]);
        } else {
            printf("%s", lines[i]);
            free(lines[i]);
        }
    }
    //and of course free memory
    free(lines);

}

//main parses the command line flags and then calls the sort function based on those flags
int main(int argc, char *argv[]) {
    //variables defining how cmd line flags will be read
    cmp_fn_t cmp = cmp_pstr;
    bool uniq = false;
    bool reverse = false;

    //based on the opt output this sets the above values accordingly
    int opt = getopt(argc, argv, "lnru");
    while (opt != -1) {
        if (opt == 'l') {
            cmp = cmp_pstr_len;
        } else if (opt == 'n') {
            cmp = cmp_pstr_numeric;
        } else if (opt == 'r') {
            reverse = true;
        } else if (opt == 'u') {
            uniq = true;
        } else {
            return 1;
        }

        opt = getopt(argc, argv, "lnru");
    }

    //sets the proper input stream (if no file named then use stdin)
    FILE *fp = stdin;
    if (optind < argc) {
        fp = fopen(argv[optind], "r");
        if (fp == NULL) {
            error(1, 0, "cannot access %s", argv[optind]);
        }
    }

    //sort the lines, close the file, call it a freakin' day
    sort_lines(fp, cmp, uniq, reverse);
    fclose(fp);
    return 0;
}
