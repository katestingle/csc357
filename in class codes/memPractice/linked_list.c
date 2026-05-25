// linked_list.c
// CSC/CPE 357 - Memory Allocation (Slide 20: Malloc + Structs -> Linked Lists)
//
// Demonstrates:
//  - heap allocation of structs (malloc/free)
//  - heap allocation of string data (strdup/free)
//  - why you must free *everything you allocate*
//
// Build:
//   gcc -Wall -Wextra -Werror -std=c11 -pedantic linked_list.c -o linked_list
// Run:
//   ./linked_list

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *data;          // heap-allocated string
    struct Node *next;   // pointer to next node (or NULL)
} Node;

typedef struct {
    Node *head;         
    size_t size;
} List;

static void die_oom(void) {
    fprintf(stderr, "Error: out of memory\n");
    exit(EXIT_FAILURE);
}

static void list_init(List *list) {
    list->head = NULL;
    list->size = 0;
}

static Node *node_create(const char *s) {
    // Allocate the node itself on the heap
    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL) die_oom();

    // Allocate/copy the string on the heap
    // strdup() returns a heap-allocated copy you must free()
    n->data = strdup(s);
    if (n->data == NULL) {
        free(n);
        die_oom();
    }

    n->next = NULL;
    return n;
}

static void list_push_front(List *list, const char *s) {
    Node *n = node_create(s);
    n->next = list->head;
    list->head = n;
    list->size++;
}

static void list_append(List *list, const char *s) {
    Node *n = node_create(s);

    if (list->head == NULL) {
        list->head = n;
        list->size++;
        return;
    }

    Node *cur = list->head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = n;
    list->size++;
}

static const Node *list_find(const List *list, const char *key) {
    for (const Node *cur = list->head; cur != NULL; cur = cur->next) {
        if (strcmp(cur->data, key) == 0) {
            return cur;
        }
    }
    return NULL;
}

// Remove the first node whose data matches key.
// Returns 1 if removed, 0 if not found.
static int list_remove_first(List *list, const char *key) {
    Node *cur = list->head;
    Node *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->data, key) == 0) {
            if (prev == NULL) {
                // removing head
                list->head = cur->next;
            } else {
                prev->next = cur->next;
            }

            // IMPORTANT: free in reverse order of ownership
            free(cur->data);
            free(cur);
            list->size--;
            return 1;
        }

        prev = cur;
        cur = cur->next;
    }
    return 0;
}

static void list_print(const List *list) {
    printf("List (size=%zu): ", list->size);
    const Node *cur = list->head;
    while (cur != NULL) {
        printf("\"%s\"", cur->data);
        cur = cur->next;
        if (cur != NULL) printf(" -> ");
    }
    printf(" -> NULL\n");
}

static void list_free(List *list) {
    Node *cur = list->head;
    while (cur != NULL) {
        Node *next = cur->next;

        // Each node owns its string copy
        free(cur->data);
        free(cur);

        cur = next;
    }

    list->head = NULL;
    list->size = 0;
}

int main(void) {
    List list;
    list_init(&list);

    // Mix of front + append to show pointer manipulation
    list_append(&list, "malloc");
    list_append(&list, "free");
    list_push_front(&list, "struct");
    list_append(&list, "strdup");

    list_print(&list);

    const char *key = "free";
    printf("Find \"%s\": %s\n", key, list_find(&list, key) ? "FOUND" : "NOT FOUND");

    printf("Remove \"%s\": %s\n", key, list_remove_first(&list, key) ? "REMOVED" : "NOT FOUND");
    list_print(&list);

    // Clean up: avoid memory leaks
    list_free(&list);

    return 0;
}