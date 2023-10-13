#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <error.h>

enum { SORT_BY_NAME, SORT_BY_TYPE };
enum { EXCLUDE_DOT, INCLUDE_DOT };

/* On the myth filesystem, the only file type information that is accurate is
 * directory/not-directory used here. Other type info in struct dirent is 
 * not reliable.
 */
bool is_dir(const struct dirent *dirptr) {
    return dirptr->d_type == DT_DIR;
}


//by_name is a sorting function that sorts directory contents lexicographically by name
int by_name(const struct dirent **a, const struct dirent **b) {
    const char *str1 = (*a)->d_name;
    const char *str2 = (*b)->d_name;

    return strcmp(str1, str2);
}

/*by_type sorts based on whether pointees are directories or files
 * directories precede files and objects of the same type are sorted by name
 */
int by_type(const struct dirent **a, const struct dirent **b) {
    const struct dirent *obj1 = *a;
    const struct dirent *obj2 = *b;

    bool dir_obj1 = is_dir(obj1);
    bool dir_obj2 = is_dir(obj2);
    
    if (dir_obj1 == dir_obj2) {
        return by_name(&obj1, &obj2);
    }
    return (dir_obj1) ? -1 : 1;
}

//no_dot returns the name of non-hidden objects
int no_dot(const struct dirent *ptr) {
    //if the object name starts with a dot, we don't want it!
    return (ptr->d_name[0] != '.');
}

//the ls function uses scandir to sort the contents of a directory and then print the contents
void ls(const char *dirpath, int filter, int order) {
    struct dirent **nameList;    //the list of items in the dir will go in here
    int n;

    int (*filt)(const struct dirent *) = (filter) ? NULL : no_dot;
    int (*ord)(const struct dirent **, const struct dirent **) = (order) ? by_type : by_name;
    n = scandir(dirpath, &nameList, filt, ord);
    //check if they want to ls an invalid object
    if (n == -1) {
        error(0, 0, "cannot access %s", dirpath);
        return;
    }
    //loop over nameList to print out the items within dirPath
    for (int i = 0; i < n; i++) {
        printf("%s", nameList[i]->d_name);
        if (is_dir(nameList[i])) {
                printf("/");
        }
        printf("\n");
        //free up all the memory at the indicies of nameList
        free(nameList[i]);
    }
    //free up the actual nameList
    free(nameList);
}

//figure out what's going on with the command line flags and then call the ls fuction accordingly
int main(int argc, char *argv[]) {
    
    int order = SORT_BY_NAME;
    int filter = EXCLUDE_DOT;

    int opt = getopt(argc, argv, "az");    //opt is the ascii val of first flag, -1 if no flags there
    //if there are flags
    while (opt != -1) {
        if (opt == 'a') {
            filter = INCLUDE_DOT;    //if we've detected -a then we change our filter method
        } else if (opt == 'z') {
            order = SORT_BY_TYPE;    //if we detect -z then we change our sorting method
        } else {
            return 1;
        }

        opt = getopt(argc, argv, "az");
    }

    //if there are several paths to ls
    if (optind < argc - 1) {
        for (int i = optind; i < argc; i++) {
            printf("%s:\n", argv[i]);    //print the name of the path
            ls(argv[i], filter, order);    //ls that path with our decided filter and order specs
            printf("\n");
        }
    } else {
        //ternary chooses current dir (denoted by ".") if no path is listed
        ls(optind == argc -1 ? argv[optind] : ".", filter, order);    //ls the path, no need to id it
    }
    
    return 0;
}
