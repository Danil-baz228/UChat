#include "libmx.h"



static void swap(void **s1, void **s2) {
    void *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

t_list *mx_sort_list(t_list *lst, bool (*cmp)(void *, void *)) {
    if (!lst) {
        return NULL; 
    }

    bool swapped;
    do {
        swapped = false;
        t_list *current = lst;
        while (current->next) {
            if (cmp(current->data, current->next->data)) {
                swap(&current->data, &current->next->data);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);

    return lst;
}



