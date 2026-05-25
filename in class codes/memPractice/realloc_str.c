#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// compare function to be used by qsort
int compare_str(const void *a, const void *b) {
    char *str_a = *((char**) a);
    char *str_b = *((char**) b);
    return strcmp(str_a, str_b);
}

int main() {

    int count = 0;
    char **strings = NULL;  // a dynamically-sized array of char *
    char buf[100];
    
    printf("Provide one string (less than 100 characters) per line, EOF to quit.\n");
    
    while (fgets(buf, 99, stdin)) {
        // resize the char * array to accomodate this new string
        void *tmp = realloc(strings, (count + 1) * sizeof(char *));
        printf("realloc() returned %p, size: %lu\n", tmp, (count + 1) * sizeof(int));
        
        if (tmp == NULL) {
            // realloc failed
            // free strings (if non-NULL)
            printf("realloc() failed()\n");
            return EXIT_FAILURE;
            /*
        } else if (tmp == strings) {
            // realloc returned same space, strings pointer can remain unchanged
            strings[count++] = buf;
            */
        } else {
            // realloc returned different block of memory, need to update strings pointer
            strings = tmp;
            strings[count++] = buf;
            
            // malloc (strlen(buf) + 1)
            // strcpy(...)
            // assign the pointer
        }
    }

    // sort the array using qsort() function
    qsort(strings, count, sizeof(char*), compare_str);

    printf("Your %d strings (sorted): \n", count);
    
    for (int i = 0; i < count; i++) {
        printf("%s", strings[i]);
        //free( strings[i]);  // malloc implicitly done by strdup()
    }
    
    free(strings);  // free the heap-allocated array of strings

    return EXIT_SUCCESS;

}
