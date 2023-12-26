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

/// @brief represents all the possible `data types` that the `list functions` can handle.
/// This is required for the `printing functions` that should know somehow what the type
/// of the `data` field is to print it correctly. Also some `searching functions` take
/// this enum as a parameter to know what to look for. `Note: not all the types are implemented
/// by default`. Only the `integer`, `char`, `float` and `string` because they're the most common ones.
/// The implementation is still in process.
typedef enum _list_data_type {
    INT,
    CHAR,
    FLOAT,
    STRING,
    DOUBLE,
    LONG_INT,
    SHORT_INT,
    LONG_DOUBLE,
    SIGNED_CHAR,
    UNSIGNED_INT,
    UNSIGNED_CHAR,
    LONG_LONG_INT,
    UNSIGNED_LONG_INT,
    UNSIGNED_LONG_LONG_INT,
} ListDataType;


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

/// @brief allocates 1 node with the `data` and pushes it to the list pointed by the `root` in a specific `index`.
/// @param root a pointer to the beginning of the list.
/// @param data the data that a new node will be initialized with.
/// @param index a place where the node will be set in a list `(starts from 0)`.
/// @return `true` if a node was pushed successfully. If `root` is `NULL` or points to `NULL`, the function
/// returns `false`. Except if `index` is equal to 0. In this case, if the `root` points to `NULL`, the 
/// function will push the node (basically working as a `list_push_front`) and return `true`. 
/// Also `false` is returned if the `index` exceeds the size of the list. The `errno` value will be set
/// to: `EINVAL` if the `root` pointer is `NULL`, `ENOMEM` if allocations failed. Note: if a new node was
/// allocated but the place for it wasn't found, then it will be freed using the WEAK mode. If `data` is
/// a dynamically allocated, it'll have to be freed manually.
bool list_push_by_index(List **root, void *data, size_t index);

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
/// (check the `DeallocationMode` enum typedefinition for more information).
/// @return `true` if the first node was freed successfully. Or `false` if the `root` pointer is `NULL` or if
/// the `*root` points to an empty list (`NULL`). In this case the `errno` will be set to `EINVAL`.
bool list_pop_front(List **root, DeallocationMode mode);

/// @brief deallocates a node from a list pointed by `root` in a specific `index` using a deallocation `mode`.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction for a function on how to release memory.
/// (check the `DeallocationMode` enum typedefinition for more information).
/// @param index an index of the node that should be freed.
/// @return `true` if the node was successfully freed. `false` is returned in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (`errno` will be set to `EINVAL`); 
/// if the `index` is greater than the size of the list (`errno` won't be changed);
bool list_pop_by_index(List **root, DeallocationMode mode, size_t index);

/* -- -- */

/* -- FUNCTIONS THAT WORK WITH THE WHOLE LIST -- */

/// @brief deallocates the whole list pointed by the `root` using a specific `mode`.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to free the memory inside a node and the node itself.
/// (check the `DeallocationMode` enum typedefenition for more information).
/// @return `true` if successfully freed the list. If the `root` pointer is invalid, `false` will be returned.
bool list_deallocate(List **root, DeallocationMode mode);

/// @brief converts all the dynamically allocated strings in a list into int-pointers.
/// Note: for every node there will be 1 integer allocated on the heap and the memory for
/// the strings will be released. When the list is no longer needed, it should be freed
/// using the `STRONG` mode (check the `DeallocationMode` enum typedefinition for more information).
/// @param root a pointer to the beginning of the list.
/// @return `true` if everything was converted successfully and `false` in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (the `errno` value will he set to `EINVAL`);
/// if allocating an integer to replace a string failed (the `errno` value will be set to `ENOMEM`);
/// if a string from a node is not a valid integer (`errno` value would be replaced with `EINVAL`).
bool list_convert_strings_to_int_ptrs(List **root);

/// @brief removes the first duplicating integer in the list.
/// @param root a pointer to the beginning of the list.
/// @param mode a specifier on how to release memory in the node.
/// In case of a node stores a pointer to a dynamically allocated integer,
/// then the `STRONG` mode should be used. Otherwise, if the node stores
/// a pointer to an integer that was allocated on a stack, then the `WEAK` mode should be used.
/// @return `true` if any duplicates were removed or `false` in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (the `errno` value is set to be `EINVAL`);
bool list_remove_duplicate_int(List **root, DeallocationMode mode);

/// @brief removes the first duplicate of the specific `data_type` in the list.
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to release the data field's memory. Note that
/// if the `mode` is `WEAK` and the `data_type` is a `POINTER`, it's going to cause
/// a memory leak. (check the `DeallocationMode` enum definition for more information).
/// @param data_type the data's type of a node. Implemented for: INT
/// (check the `ListDataType` enum definition for more information).
/// @return `true` if any duplicates were removed or `false` in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (the `errno` value is set to be `EINVAL`);
/// if the `data_type` value is not implemented for a specific type (the `errno` value is set to be ` ECANCELED`);
bool list_remove_duplicate(List **root, DeallocationMode mode, ListDataType data_type);

/// @brief removes all the duplicates from the list that contain data with the specific `data_type`
/// and the `mode` to release unused memory. 
/// @param root a pointer to the beginning of the list.
/// @param mode an instruction on how to release the data field's memory. Note that
/// if the `mode` is `WEAK` and the `data_type` is a `POINTER`, it's going to cause
/// a memory leak. (check the `DeallocationMode` enum definition for more information).
/// @param data_type the data's type of a node. Implemented for the same types as in 
/// the `list_remove_duplicate` function.
/// (check the `ListDataType` enum definition for more information).
/// @return the same values as the `list_remove_duplicate` function. 
bool list_remove_duplicates(List **root, DeallocationMode mode, ListDataType data_type);

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
/// @return a pointer to a node that in which the `data` field stores a pointer to a string with a 
/// specific `value`. If either the `root` is `NULL` or the list is empty or the `value` wasn't found, `NULL` is returned.
List *list_get_by_string_value(List **root, char *string);

/// @brief searches for the `value` within the list interpreting each node `data` field as the `data_type`.
/// Note: this function is an abstraction on top of other functions like `list_get_by_<type>_value`. It is
/// recommended to use those functions rather than this one because it does extra work to identify the type
/// to look for. This function also can be used in general cases if the node types are not known and the
/// explicit search functions can't be used.
/// @param root the beginning of the list.
/// @param value a value to look for in the list.
/// @param data_type a data type specifier that explicitly tells the function what type to look for.
/// (check the `ListDataType` enum definition for more information).
/// @return a pointer to a node that in which the `data` field stores a pointer to a string with a 
/// specific `value`. If either the `root` is `NULL` or the list is empty or the `value` wasn't found, `NULL` is returned.
List *list_get_by_value(List **root, void *value, ListDataType data_type);

/// @brief returns a pointer to the last element of the list.
/// @param root a pointer to the beginning of the memory.
/// @return a pointer to the last node. If the `root` pointer is `NULL` or the list is empty, 
/// `NULL` is returned and the `errno` is set to be `EINVAL`.
List *list_get_last(List **root);

/// @brief searches for the max integer in a list and sets its value to the `max` pointer.
/// @param root a pointer to the begisnning of the list.
/// @param max a pointer that will store the found max integer.
/// @return `true` if the integer was found and `false` in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (the `errno` value will be set to `EINVAL`);
/// if the `max` pointer is `NULL`(the `errno` value will be set to `EINVAL`);
bool list_get_max_int(List **root, int *max);

/// @brief searches for the min integer in a list and sets its value to the `min` pointer.
/// @param root a pointer to the begisnning of the list.
/// @param max a pointer that will store the found min integer.
/// @return `true` if the integer was found and `false` in case of any errors such as:
/// if the `root` pointer is `NULL` or points to `NULL` (the `errno` value will be set to `EINVAL`);
/// if the `min` pointer is `NULL`(the `errno` value will be set to `EINVAL`);
bool list_get_min_int(List **root, int *min);

/// @brief returns a length of the list.
/// @param root a pointer to the beginning of the list.
/// @return if everything was done successfully, the length is returned. `0` in case of any errors such as:
/// the `root` pointer is `NULL` (the `errno` value is set to be `EINVAL`). Note: if the `root` points to
/// `NULL` (aka an empty list), it is not considered as an error so the `errno` value is not going to change
/// and `0` is going to be returned.
size_t list_get_length(List **root);

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

/// @brief prints the node data as a string in the following format: `{ value: <data>, next: <ptr> } <newline>`
/// @param node a node to print
/// @return `true` if printed successfully and `false` in case of any errors such as:
/// if the trimming newlines failed and couldn't allocate the memory for a temporary string (the `errno` value is set to be `ENOMEM`);
bool list_print_node_string(List **node);

/// @brief prints the `node` in the following format: `{ value: <data>, next: <ptr> } <newline>`.
/// @param node a pointer to a node to print.
/// @param data_type a data type specifier that explicitly tells the function how to print.
/// the data field (check the `ListDataType` enum definition for more information).
/// @return `true` if printed successfully or `false` in case of any errors such as:
/// if the `node` pointer is `NULL` (the `errno` value is set to be `EINVAL`);
/// if the `data_type` is not implemented for a specific type (the `errno` value is set to be `ECANCELED`);
bool list_print_node(List **node, ListDataType data_type);

/// @brief prints the whole list. 
/// @param node a pointer to the beginning of the list.
/// @param data_type a data type specifier that explicitly tells the function how to print.
/// the data field (check the `ListDataType` enum definition for more information).
/// @return `true` if everything was printed successfully or `false` in case of any errors.
/// the errors are the same as in the `list_print_node` function. Check its definition for more information.
bool list_print(List **root, ListDataType data_type);

/* -- -- */

/* -- ERROR HANDLING -- */

/// @brief prints the errno message converted to a string into the stderr
void list_print_error();

/* -- -- */

/* -- FOREACH FUNCTIONS -- */

/// @brief applies the `function` for each node in the list.
/// @param root a pointer to the beginning of the list
/// @param function a pointer to a function that accepts a node.
/// @return `true` if executed successfully, `false` in case of any errors such as:
/// if the `root` pointer is `NULL` (the `errno value is set to be `EINVAL`);
/// if the `function` pointer is `NULL` (the `errno value is set to be `EINVAL`);
bool list_foreach_node(List **root, void (*function)(List *));

/* -- -- */

#ifdef LIST_IMPLEMENTATION

/* -- NODE ALLOCATION FUNCTIONS -- */

List *list_allocate_node(void *data)
{
    List *node = LIST_MALLOC_ITEM(List);

    if (!node) LIST_SET_ERRNO_END_RETURN(ENOMEM, false);

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

    List *last = list_get_last(root);

    if (!last) {
        *root = new_node;
    } else { 
        last->next = new_node;
    }

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

bool list_push_by_index(List **root, void *data, size_t index)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    if (!(*root)) {
        if (index != 0) return false;
        return list_push_front(root, data);
    }

    if (index == 0) return list_push_front(root, data);

    List *new_node = list_allocate_node(data);

    if (!new_node) LIST_SET_ERRNO_END_RETURN(ENOMEM, false);

    List *previous_node = list_get_by_index(root, index - 1);
    List *next_node = previous_node->next;

    previous_node->next = new_node;
    new_node->next = next_node;

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

bool list_pop_by_index(List **root, DeallocationMode mode, size_t index)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    if (index == 0) return list_pop_front(root, mode);

    List *previous_node = list_get_by_index(root, index - 1);

    if (!previous_node || !previous_node->next) return false;

    List *target_node = previous_node->next;
    List *after_target_node = target_node->next;

    list_deallocate_node(&target_node, mode);

    previous_node->next = after_target_node;

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

bool list_convert_strings_to_int_ptrs(List **root)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    bool result = true;

    for (List *i = *root; i; i = i->next) {
        int *integer = LIST_MALLOC_ITEM(int);

        if (!integer) LIST_SET_ERRNO_END_RETURN(ENOMEM, false);

        const char *integer_string = (const char *)i->data;

        *integer = atoi(integer_string);

        if (*integer == 0 && strncmp(integer_string, "0", 1) != 0) {
            errno = EINVAL;
            result = false;
        }

        free(i->data);

        i->data = integer;
    }

    return result;
}

bool list_remove_duplicate_int(List **root, DeallocationMode mode)
{
    if (!root || !(*root)) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    size_t index = 0;

    for (List *i = *root; i; i = i->next) {
        int i_as_int = *(int *)i->data;
        size_t temp = index + 1;

        for (List *j = i->next; j; j = j->next) {
            int j_as_int = *(int *)j->data;

            if (i_as_int != j_as_int) {
                temp++;
                continue;
            }

            list_pop_by_index(root, mode, temp);
            return true;    
        }

        index++;
    }

    return false;
}

bool list_remove_duplicate(List **root, DeallocationMode mode, ListDataType data_type)
{
    switch (data_type)
    {
        case INT: return list_remove_duplicate_int(root, mode);
        
        default:
        errno = ECANCELED;
        list_print_error();
        return false;
    }
}

bool list_remove_duplicates(List **root, DeallocationMode mode, ListDataType data_type)
{
    bool removed = list_remove_duplicate(root, mode, data_type);

    while (removed) {
        removed = list_remove_duplicate(root, mode, data_type);
    }
    
    return (errno == 0);
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

        if (current_index > index) {
            return NULL;
        }
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

List *list_get_by_value(List **root, void *value, ListDataType data_type)
{
    switch (data_type)
    {
        case INT:    return list_get_by_integer_value(root, *(int *)value); 
        case STRING: return list_get_by_string_value(root, (char *)value); 

        default:   
            errno = ECANCELED;
            list_print_error();
            break;
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

bool list_get_max_int(List **root, int *max)
{
    if (!root || !(*root) || !max) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    *max = *(int *)(*root)->data;

    for (List *i = (*root)->next; i; i = i->next) {
        int temp = *(int *)i->data;

        if (temp > *max) *max = temp;
    }

    return true;
}

bool list_get_min_int(List **root, int *min)
{
    if (!root || !(*root) || !min) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    *min = *(int *)(*root)->data;

    for (List *i = (*root)->next; i; i = i->next) {
        int temp = *(int *)i->data;

        if (temp < *min) *min = temp;
    }

    return true;
}

size_t list_get_length(List **root)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, 0);

    size_t length = 0;

    for (List *i = *root; i; i = i->next) {
        length++;
    }

    return length;
}

/* -- -- */

/* -- IO FUNCTIONS -- */

List *list_read_line_as_string(size_t buffer_size)
{
    if (buffer_size == 0) LIST_SET_ERRNO_END_RETURN(EINVAL, NULL);

    List *result = list_allocate_node(NULL);

    if (!result) LIST_SET_ERRNO_END_RETURN(ENOMEM, NULL);

    char *input_string = LIST_MALLOC_ITEMS(char, buffer_size);

    if (!input_string) {
        errno = ENOMEM;
        list_deallocate_node(&result, WEAK);
        return NULL;
    }

    if (!fgets(input_string, buffer_size, stdin)) {
        errno = EIO;
        list_deallocate_node(&result, WEAK);
        free(input_string);
        return NULL;
    }

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

bool list_print_node_string(List **node)
{
    if (!node) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    printf("{ value: ");

    char *str = (char *)(*node)->data;

    if (!str) {
        printf("(null), next: %p }\n", (*node)->next);
        return true;
    }

    printf("`");

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            printf("[NL]");
            continue;
        }

        printf("%c", str[i]);
    }

    printf("`, next: %p }\n", (*node)->next);

    return true;
}

bool list_print_node(List **node, ListDataType data_type)
{
    if (!node) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

            char *node_string_without_newlines = NULL;
    switch (data_type) {
        case INT: 
            printf("{ value: %d, next: %p }\n", *(int *)(*node)->data, (*node)->next);
            break;
        case FLOAT: 
            printf("{ value: %f, next: %p }\n", *(float *)(*node)->data, (*node)->next);
            break;
        case CHAR: 
            printf("{ value: %c, next: %p }\n", *(char *)(*node)->data, (*node)->next);
            break;
        case STRING: return list_print_node_string(node);
        default:
            errno = ECANCELED;
            list_print_error();
            break;
    }

    return (errno == 0);
}

bool list_print(List **root, ListDataType data_type)
{
    if (!root) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    printf("{\n");
    for (List *i = *root; i; i = i->next) {
        printf("\t");
        if (!list_print_node(&i, data_type)) {
            printf("\n");
            return false;
        }
    }
    printf("}\n");

    return true;
}

/* -- -- */

/* -- ERROR HANDLING -- */

void list_print_error()
{
    fprintf(stderr, "[LIST][ERROR]: %s\n", strerror(errno));
}

/* -- -- */

/* -- FOREACH FUNCTIONS -- */


bool list_foreach_node(List **root, void (*function)(List *))
{
    if (!root || !function) LIST_SET_ERRNO_END_RETURN(EINVAL, false);

    for (List *i = *root; i; i = i->next) {
        function(i);
    }

    return true;
}

/* -- -- */

#endif // LIST_IMPLEMENTATION