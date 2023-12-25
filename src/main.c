#define LIST_IMPLEMENTATION
#include "./list.h"

#define N 2
#define MAX_INPUT_STRING_SIZE 12

int main()
{
    List *numbers = list_read_lines_as_string(MAX_INPUT_STRING_SIZE, N);

    if (!list_convert_strings_to_int_ptrs(&numbers)) goto error;

    int max = 0;
    if (!list_get_max_int(&numbers, &max)) goto error;

    printf("Max number: %d\n", max);
    list_deallocate(&numbers, STRONG);
    return 0;

error:
    list_print_error();
    list_deallocate(&numbers, STRONG);
    return 1;
}