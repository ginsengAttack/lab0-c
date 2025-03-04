#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (new == NULL)
        return NULL;
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *pos = head->next;
    while (pos != head) {
        list_del(pos);
        element_t *node = list_entry(pos, element_t, list);
        free(node->value);
        free(node);
        pos = head->next;
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || strlen(s) == 0)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || strlen(s) == 0)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    strncpy(sp, node->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&node->list);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    strncpy(sp, node->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&node->list);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *slow = head->next;
    const struct list_head *pos;
    for (pos = head->next; pos->next != head && pos->next->next != head;
         pos = pos->next->next)
        slow = slow->next;
    list_del(slow);
    free(list_entry(slow, element_t, list)->value);
    free(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head)
        return false;
    q_sort(head, false);
    struct list_head **cur = &head->next;
    while ((*cur)->next != head && *cur != head) {
        if (strcmp(list_entry(*cur, element_t, list)->value,
                   list_entry((*cur)->next, element_t, list)->value) == 0) {
            struct list_head *tmp = (*cur)->next;
            while (tmp != head &&
                   strcmp(list_entry(tmp, element_t, list)->value,
                          list_entry(*cur, element_t, list)->value) == 0) {
                struct list_head *del = tmp;
                tmp = tmp->next;
                list_del(del);
                free(list_entry(del, element_t, list)->value);
                free(list_entry(del, element_t, list));
            }
            struct list_head *del = *cur;
            tmp->prev = (*cur)->prev;
            *cur = tmp;
            free(list_entry(del, element_t, list)->value);
            free(list_entry(del, element_t, list));
        } else
            cur = &(*cur)->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head **cur = &head->next;
    struct list_head *nex;
    for (; *cur != head && (*cur)->next != head; cur = &(*cur)->next->next) {
        nex = (*cur)->next;
        (*cur)->next = nex->next;
        nex->next->prev = *cur;
        nex->next = *cur;
        nex->prev = (*cur)->prev;
        (*cur)->prev = nex;
        *cur = nex;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur = head->next, *nex;
    for (nex = cur->next; cur != head;) {
        list_move(cur, head);
        cur = nex;
        nex = cur->next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
struct list_head *merge_sort(struct list_head *head)
{
    int len = q_size(head);
    if (len <= 1) {
        if (len == 0)
            return head;
        else if (strcmp(list_entry(head, element_t, list)->value,
                        list_entry(head->next, element_t, list)->value) > 0) {
            list_move(head, head->next);
            return head->next;
        }
        return head;
    }
    struct list_head *list1 = head;
    struct list_head *list2 = head;
    for (int i = len / 2; i >= 0; i--)
        list2 = list2->next;
    struct list_head *tmp = list2->prev;
    list2->prev = list1->prev;
    list1->prev = tmp;
    list1->prev->next = list1;
    list2->prev->next = list2;
    list1 = merge_sort(list1);
    list2 = merge_sort(list2);
    struct list_head *trace1 = list1;
    struct list_head *trace2 = list2;
    bool list1_bool = false, list2_bool = false;
    head = strcmp(list_entry(list1, element_t, list)->value,
                  list_entry(list2, element_t, list)->value) <= 0
               ? list1
               : list2;
    do {
        if (trace1 == head && list1_bool) {
            struct list_head *tmp2 = trace2;
            trace2 = trace2->next;
            list_add_tail(tmp2, trace1);
            list2_bool = true;
        } else if (strcmp(list_entry(trace1, element_t, list)->value,
                          list_entry(trace2, element_t, list)->value) <= 0) {
            trace1 = trace1->next;
            list1_bool = true;
        } else {
            struct list_head *tmp2 = trace2;
            trace2 = trace2->next;
            list_add_tail(tmp2, trace1);
            list2_bool = true;
        }
    } while (trace2 != list2 || !list2_bool);
    return head;
}
void q_sort(struct list_head *head, bool descend)
{
    struct list_head *list = head->next;
    list_del(head);
    list = merge_sort(list);
    list_add_tail(head, list);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int number = 0;
    const char *test = list_entry(head->prev, element_t, list)->value;
    for (struct list_head *pos = head->prev; pos != head;) {
        if (strcmp(test, list_entry(pos, element_t, list)->value) < 0) {
            struct list_head *del = pos;
            pos = pos->prev;
            list_del(del);
            free(list_entry(del, element_t, list)->value);
            free(list_entry(del, element_t, list));
        } else {
            test = list_entry(pos, element_t, list)->value;
            number++;
            pos = pos->prev;
        }
    }
    return number;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int number = 0;
    const char *test = list_entry(head->prev, element_t, list)->value;
    for (struct list_head *pos = head->prev; pos != head;) {
        if (strcmp(test, list_entry(pos, element_t, list)->value) > 0) {
            struct list_head *del = pos;
            pos = pos->prev;
            list_del(del);
            // free(list_entry(del,element_t,list)->value);
            // free(list_entry(del,element_t,list));
        } else {
            test = list_entry(pos, element_t, list)->value;
            number++;
            pos = pos->prev;
        }
    }
    return number;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
