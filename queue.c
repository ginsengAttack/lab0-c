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
void reverse(struct list_head *head, struct list_head *tail)
{
    struct list_head *left = head;
    tail = tail->next;
    do {
        struct list_head *tmp = head->next;
        list_del(head->next);
        list_add_tail(tmp, left);
        left = left->prev;
    } while (head->next != tail);
}
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (k == 1 || !head || head->next == head)
        return;
    for (struct list_head *pos = head->next; pos != head;) {
        struct list_head *tail = pos;
        for (int i = 0; i < k - 1; i++) {
            tail = tail->next;
            if (tail == head)
                return;
        }
        reverse(pos, tail);
        pos = pos->next;
    }
}

/* Sort elements of queue in ascending/descending order */
void merge_two(struct list_head *list1, struct list_head *list2)
{
    struct list_head *list1_pos = list1->next;
    for (struct list_head *pos = list2->next; pos != list2;) {
        if (list1_pos != list1 &&
            strcmp(list_entry(list1_pos, element_t, list)->value,
                   list_entry(pos, element_t, list)->value) < 0)
            list1_pos = list1_pos->next;
        else {
            struct list_head *add = pos;
            pos = pos->next;
            list_del(add);
            list_add_tail(add, list1_pos);
        }
    }
}
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next->next == head)
        return;

    struct list_head *list2 = head;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        list2 = list2->next;
        fast = fast->next->next;
    }

    struct list_head left;
    list_cut_position(&left, head, list2);
    q_sort(head, descend);
    q_sort(&left, descend);
    merge_two(head, &left);
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

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || head->next == head)
        return 0;
    else if (head->next->next == head)
        return list_entry(head->next, queue_contex_t, chain)->size;
    struct list_head *first = list_entry(head->next, queue_contex_t, chain)->q;
    for (struct list_head *pos = head->next->next; pos != head;
         pos = pos->next) {
        if (list_entry(pos, queue_contex_t, chain)->size != 0) {
            merge_two(first, list_entry(pos, queue_contex_t, chain)->q);
            list_entry(head->next, queue_contex_t, chain)->size +=
                list_entry(pos, queue_contex_t, chain)->size;
            list_entry(pos, queue_contex_t, chain)->size = 0;
        }
    }
    return list_entry(head->next, queue_contex_t, chain)->size;
}