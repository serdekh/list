#define LIST_IMPLEMENTATION
#include "./list.h"

#define N 3
#define ATOI_FAILED(n, node) (n == 0 && strcmp((const char *)node->data, "0") != 0)

int main()
{
    List *numbers = list_read_lines_as_string(10, N);
  
    int max = atoi((const char *)(numbers->data));

    if (ATOI_FAILED(max, numbers)) {
        fprintf(stderr, "ERROR: invalid input for the 1th number: `%s`\n", (char *)numbers->data);
        list_deallocate(&numbers, STRONG);
        return 1;
    }

    size_t index = 1;

    for (List *i = numbers->next; i; i = i->next) {
        int temp = atoi((const char *)i->data);

        if (ATOI_FAILED(temp, i)) {
            fprintf(stderr, "ERROR: invalid input for the %zuth number: `%s`\n", index, (char *)numbers->data);
            list_deallocate(&numbers, STRONG);
            return 1;
        }

        if (temp > max) max = temp;
    
        index++;
    }

    printf("Max number: %d\n", max);
    list_deallocate(&numbers, STRONG);

    return 0;
}