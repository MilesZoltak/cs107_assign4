/* This file includes test code for the binsert
 * function implemented as part of assign4.
 * You do not need to modify this file.
 * You can run this program by specifying the -i flag
 * to parse as integers, or -s to parse as strings.  This
 * determines how the remaining command-line arguments are
 * processed.  Then, the remaining arguments are inserted
 * in order into an array via binsert.
 */

#include "samples/prototypes.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

int cmp_int(const void *p, const void *q) {
    int first = *(const int *)p;
    int second = *(const int *)q;
    return first - second;
}

/* Test building up an array by parsing each string argument as an int
 * in order and inserting it using binsert.  Asserts if each
 * element was inserted/handled correctly, and prints out the
 * built up array at the end.
 */
void test_int(int count, char *args[]) {
    int arr[count + 1];
    size_t nused = 0;

    for (int i = 0; i < count; i++) {
        int val = atoi(args[i]);
        int *added = binsert(&val, arr, &nused, sizeof(arr[0]), cmp_int);
        assert(val == *added);
    }

    for (int i = 0; i < nused; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

int cmp_str(const void *p, const void *q) {
    const char *first = *(const char **)p;
    const char *second = *(const char **)q;
    return strcmp(first, second);
}

/* Test building up an array with each string argument
 * inserted in order using binsert.  Asserts if each
 * element was inserted/handled correctly, and prints out the
 * built up array at the end.
 */
void test_string(int count, char *args[]) {
    char *arr[count + 1];
    size_t nused = 0;

    for (int i = 0; i < count; i++) {
        char *val = args[i];
        char **added = binsert(&val, arr, &nused, sizeof(arr[0]), cmp_str);
        assert(strcmp(val, *added) == 0);
    }

    for (int i = 0; i < nused; i++) {
        printf(" %s", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    bool is_string = false;
    bool is_int = false;

    
    int opt = getopt(argc, argv, "is");
    //while there are flags to check
    while (opt != -1) {
        if (opt == 'i') {
            is_int = true;    //if -i is read we are working with ints
        } else if (opt == 's') {
            is_string = true;    //if -s is read we are working with strings
        } else {
            return 1;    //exit w failure bc we passed invalid flag
        }

        opt = getopt(argc, argv, "is");    //i guess we can check again
    }

    //default to string if both true, otherwise test if -i or -s is called
    if (is_string) {
        test_string(argc - optind, argv + optind);
    } else if (is_int) {
        //arg 1 = non-flag args, arg2 = the list to search
        test_int(argc - optind, argv + optind);
    }

    return 0;
}
