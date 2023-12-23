#pragma once

#include <errno.h>
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

#define LIST_MALLOC_ITEM(type) ((type *)malloc(sizeof(type)))

#define LIST_SET_ERRNO_END_RETURN(errno_value, return_value) ({ errno = errno_value; return return_value; })

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

#ifdef LIST_IMPLEMENTATION

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

#endif // LIST_IMPLEMENTATION