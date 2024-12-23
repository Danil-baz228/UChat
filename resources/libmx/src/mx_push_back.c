#include "libmx.h"

void mx_push_back(t_list **list, void *data) {
    if (list == NULL) {
        return;
    }

    t_list *node = mx_create_node(data);
    if (node == NULL) {
        return; 
    }

    if (*list == NULL) {
        *list = node; 
        return;
    }

    t_list *current = *list;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = node;
}


