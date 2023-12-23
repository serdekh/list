#pragma once

#include <errno.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/// @brief describes a node from a list where the `data` field stores the actual data and the `next` field points 
/// to the next element in the list. If a node is the last element in the list, the `next` field points to `NULL`
typedef struct _list {
    void *data;
    struct _list *next;
} List;

/// @brief describes in which way a node should be deallocated. 
/// `WEAK` means that the node will be freed but the `data` field won't. 
/// For example, when a node stores a pointer to an element that was allocated on a stack: `int`, `float`, `char`, `double`, `...`. 
/// `STRONG` means that the node will be freed and the `data` field as well. 
/// For example, when a node stores a pointer to an element that was allocated on a heap: `int *`, `float *`, `char *`, `double *`, `...`. 
typedef enum _deallocation_mode {
    WEAK,
    STRONG
} DeallocationMode;

#define LIST_MALLOC_ITEMS(type, count) ((type *)malloc(sizeof(type) * count))
#define LIST_MALLOC_ITEM(type) LIST_MALLOC_ITEMS(type, 1)

#define LIST_SET_ERRNO_END_RETURN(errno_value, return_value) ({ errno = errno_value; return return_value; })

#define LIST_PRINT_NODE_VALUE_AS_INT(node) (printf("%d\n", *(int *)node->data))
#define LIST_PRINT_NODE_VALUE_AS_STRING(node) (printf("%s\n", (char *)node->data))

/* -- NODE ALLOCATION FUNCTIONS -- */

/// @brief allocates dynamic memory for 1 node and sets `data` field to be `data` from the input.
/// @param data the information that the allocated node will point to
/// @return if succeeded, a dynamically allocated node with initialized data.
/// In case of an error, the errno value will be modified and `NULL` will be returned.
List *list_allocate_node(void *data); 

/// @brief deallocates dynamically allocated pointer `node` with a specification for deallocating the data field in `mode`
/// @param node a pointer to the dynamically allocated node (`List *`) to free.  
/// @param mode specifies in which way the `data` field in the `node` should be freed
/// @return `true`| `1` if the `node` is successfully freed or `false` | `0` in case of any errors.
/// If errno header is included, the errno value is changed accordingly to the error. 
bool list_deallocate_node(List **node, DeallocationMode mode);

/* -- --*/

/* -- PUSH-POP NODES FUNCTIONS -- */

/// @brief allocates 1 node with `data` and appends it to the end of the list pointed by the `root`.
/// @param root a pointer to the beginning of the list. If it points to `NULL`, then 
/// the function will allocate memory for a node and assign it to the `*root`.
/// @param data the information that will be set to the new allocated node in the `last_node->data` field.
/// @return `true` if the new node was allocated and appended successfully. Otherwise, if the `root` pointer
/// is `NULL` or memory allocation failed, the `errno` will be set to `EINVAL` or `ENOMEM` accordingly and 
/// the `false` value will be returned.
bool list_push_back(List **root, void *data);

/// @brief allocates 1 node with `data` and appends it to the beginning of the list pointed by the `root`.
/// @param root a pointer to the beginning of the list. If it points to `NULL`, then 
/// the function will allocate memory for a node and assign it to the `*root`.
/// @param data the information that will be set to the new allocated node in the `data` field.
/// @return `true` if the new node was allocated and appended successfully. Otherwise, if the `root` pointer
/// is `NULL` or memory allocation failed, the `errno` will be set to `EINVAL` or `ENOMEM` accordingly and 
/// the `false` value will be returned.
bool list_push_front(List **root, void *data);

/// @brief deallocates the last node in the list pointed by `root` using the specific deallocation `mode`.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to free the memory inside a node and the node itself.
/// (check the `DeallocationMode` enum typedefenition for more information).
/// @return `true` if the last node was freed successfully. Or `false` if the `root` pointer is `NULL` or if
/// the `*root` points to an empty list (`NULL`). In this case the `errno` will be set to `EINVAL`.
bool list_pop_back(List **root, DeallocationMode mode);

/// @brief deallocates the first node in the list pointed by `root` using the specific deallocation `mode`
/// and reassigns the `root` to point to the second element after it.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to free the memory inside a node and the node itself.
/// (check the `DeallocationMode` enum typedefenition for more information).
/// @return `true` if the first node was freed successfully. Or `false` if the `root` pointer is `NULL` or if
/// the `*root` points to an empty list (`NULL`). In this case the `errno` will be set to `EINVAL`.
bool list_pop_front(List **root, DeallocationMode mode);

/* -- -- */

/* -- FUNCTIONS THAT WORK WITH THE WHOLE LIST -- */

/// @brief deallocates the whole list pointed by the `root` using a specific `mode`.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to free the memory inside a node and the node itself.
/// (check the `DeallocationMode` enum typedefenition for more information).
/// @return `true` if successfully freed the list. If the `root` pointer is invalid, `false` will be returned.
bool list_deallocate(List **root, DeallocationMode mode);

/* -- -- */

/* -- SEARCHING FUNCTIONS --*/

/// @brief returns a pointer to a node from the list according to its `index` (starts from `0`).
/// @param root a pointer to to the beginning of the list.
/// @param index a target node's index.
/// @return if the node is found, a pointer to it is returned. If the `root` pointer is `NULL`,
/// the `errno` value is set to `EINVAL` and `NULL` is returned. If index is greater than the
/// size of the list, then the `NULL` pointer is also returned. 
List *list_get_by_index(List **root, size_t index);

/// @brief returns a pointer to a node that stores the same integer as in `value`.
/// `WARNING: if the list contains not only integers, it may cause a segfault!`
/// @param root a pointer to the beginning of the list.
/// @param value an integer value that is going to be looked for in a list.
/// @return a pointer to a node that in which the `data` field stores a pointer to an integer with a 
/// specific `value`. If either the `root` is `NULL` or the list is empty or the `value` wasn't found, `NULL` is returned.
List *list_get_by_integer_value(List **root, int value);

/// @brief returns a pointer to a node that stores the same string as in `value`.
/// `WARNING: if the list contains not only strings, it may cause a segfault!`
/// @param root a pointer to the beginning of the list.
/// @param value a string pointer that is going to be compared using the strcmp function.
/// @return a pointer to a node that in which the `data` field stores a pointer to a strjng with a 
/// specific `value`. If either the `root` is `NULL` or the list is empty or the `value` wasn't found, `NULL` is returned.
List *list_get_by_string_value(List **root, char *string);

/// @brief returns a pointer to the last element of the list.
/// @param root a pointer to the beginning of the memory.
/// @return a pointer to the last node. If the `root` pointer is `NULL` or the list is empty, 
/// `NULL` is returned and the `errno` is set to be `EINVAL`.
List *list_get_last(List **root);

/* -- -- */

/* -- IO FUNCTIONS -- */

/// @brief reads a line from a user and returns a node with the data as a char pointer.
/// Note: a string is dynamically allocated so when the node is going to be freed, use the `STRONG` mode.
/// For more information, check the `_deallocation_mode` struct definition.
/// @param buffer_size the maximum length for the input string.
/// @return a node with the input as a string. If any errors happen, 
/// the `errno` is set to be either `EINVAL` (`buffer_size` is zero) or 
/// `ENOMEM` (failed to allocate memory for a node or an input string) or
/// `EIO`(failed to read line) and the `NULL` is returned.
List *list_read_line_as_string(size_t buffer_size);

/// @brief reads `count` lines with the `buffer_size` length and creates a list of the read input lines.
/// Note: for every read this functions uses the `list_read_line_as_string` function meaning that the list
/// will contain dynamically allocated strings in each node. So when the list is no longer needed and the 
/// memory should be released, the `STRONG` deallocation mode is required. 
/// Check the `_deallocation_mode` struct definition for more information.
/// @param buffer_size the size for each string in a node.
/// @param count the amount of lines to read.
/// @return If no errors occur, the function returns a list of read lines. Otherwise, the function will
/// return `NULL` and set the `errno` to be `EIO` or `ENOMEM` or `EINVAL` accordingly to the error. To find
/// out in which case the `errno` will be changed, check the `list_read_line_as_string` function.
List *list_read_lines_as_string(size_t buffer_size, size_t count);

/* -- -- */

//#ifdef LIST_IMPLEMENTATION

/* -- NODE ALLOCATION FUNCTIONS -- */

List *list_allocate_node(void *data)
{
    List *node = LIST_MALLOC_ITEM(List);

    if (!node) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    node->data = data;
    node->next = NULL;

    return node;
}

bool list_deallocate_node(List **node, DeallocationMode mode)
{
    if (!node) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    if (!(*node)) return false;

    if (mode == STRONG && (*node)->data) free((*node)->data);

    free((*node));
    *node = NULL;

    return true;
}

/* -- -- */

/* -- PUSH-POP NODES FUNCTIONS -- */

bool list_push_back(List **root, void *data)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    List *new_node = list_allocate_node(data);

    if (!new_node) LIST_SET_ERRNO_END_RETURN(ENOMEM, false);

    if (!(*root)) {
        *root = new_node;
        return true;
    }

    List *last_element = *root;

    while (last_element->next) {
        last_element = last_element->next;
    }

    last_element->next = new_node;
    return true;
}

bool list_push_front(List **root, void *data)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    List *new_node = list_allocate_node(data);

    if (!new_node) LIST_SET_ERRNO_END_RETURN(ENOMEM, false);

    new_node->next = *root;
    *root = new_node;
    return true;
}

bool list_pop_back(List **root, DeallocationMode mode)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    if (!(*root)->next) {
        list_deallocate_node(root, mode);
        return true;
    }

    List *previous_node = *root;
    List *current_node = previous_node->next;

    while (current_node->next) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    list_deallocate_node(&current_node, mode);
    previous_node->next = NULL;

    return true;
}

bool list_pop_front(List **root, DeallocationMode mode)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    List *second_node = (*root)->next;

    list_deallocate_node(root, mode);

    *root = second_node;

    return true;
}

/* -- -- */

/* -- FUNCTIONS THAT WORK WITH THE WHOLE LIST -- */

bool list_deallocate(List **root, DeallocationMode mode)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    while (list_pop_front(root, mode)) { }

    return true;
}

/* -- -- */

/* -- SEARCHING FUNCTIONS --*/
 
List *list_get_by_index(List **root, size_t index)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    size_t current_index = 0;

    for (List *i = *root; i; i = i->next) {
        if (index == current_index) {
            return i;
        }

        current_index++;
    }

    return NULL;
}

List *list_get_by_integer_value(List **root, int value)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    for (List *i = *root; i; i = i->next) {
        if (*(int *)i->data == value) {
            return i;
        }
    }

    return NULL;
}

List *list_get_by_string_value(List **root, char *string)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    for (List *i = *root; i; i = i->next) {
        if (strcmp((char *)i->data, string) == 0) {
            return i;
        }
    }

    return NULL;
}

List *list_get_last(List **root)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    List *last = *root;

    while (last->next) {
        last = last->next;
    }

    return last;
}

/* -- -- */

/* -- IO FUNCTIONS -- */

List *list_read_line_as_string(size_t buffer_size)
{
    if (buffer_size == 0) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    List *result = list_allocate_node(NULL);

    if (!result) LIST_SET_ERRNO_END_RETURN(ENOMEM, NULL);

    char *input_string = LIST_MALLOC_ITEMS(char, buffer_size);

    if (!input_string) LIST_SET_ERRNO_END_RETURN(ENOMEM, NULL);

    if (!fgets(input_string, buffer_size, stdin)) LIST_SET_ERRNO_END_RETURN(EIO, NULL);

    input_string[buffer_size - 1] = '\0';

    result->data = input_string;

    return result;
}

List *list_read_lines_as_string(size_t buffer_size, size_t count)
{
    if (count == 0) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    List *result = list_read_line_as_string(buffer_size);

    if (!result) return NULL;

    List *last = result;

    for (size_t i = 0; i < count - 1; i++) {
        last->next = list_read_line_as_string(buffer_size);

        if (last->next) {
            last = last->next;
            continue;
        }

        list_deallocate(&result, STRONG);
        errno = ENOMEM;
        return NULL;
    }

    return result;
}

/* -- -- */

//#endif // LIST_IMPLEMENTATION