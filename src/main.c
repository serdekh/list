#include <stdio.h>
#include <string.h>

#define LIST_IMPLEMENTATION
#include "./list.h"

int main()
{
    printf("Hello world\n");

    int a = 1;
    int b = 2;
    int c = 3;

    List *numbers = NULL;

    list_push_front(&numbers, &a);
    list_push_front(&numbers, &b);
    list_push_front(&numbers, &c);

    for (List *i = numbers; i; i = i->next) {
        printf("%d\n", *(int*)i->data);
    }
    
    list_deallocate(&numbers, WEAK);

    return 0;
}