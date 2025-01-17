#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    // return null pointer if the size for malloc is 0
    if (head != NULL)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    // traverse and delete all entries
    element_t *entry = NULL, *succ = NULL;
    list_for_each_entry_safe (entry, succ, l, list) {
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    // if the string has a length = 0
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
    if (!head)
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
    if (!head || list_empty(head))
        return NULL;

    // extract the first element and unlink it from the list
    element_t *first = list_first_entry(head, element_t, list);
    list_del(head->next);

    // if sp is non-null and an element is removed, copy removed string
    if (sp) {
        strncpy(sp, first->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *last = list_last_entry(head, element_t, list);
    list_del(head->prev);

    if (sp) {
        strncpy(sp, last->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return last;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int count = 0;
    struct list_head *node;
    list_for_each (node, head)
        ++count;
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    // because this is a circular linked list
    struct list_head *target = head->next, *pre = head->prev;
    while (target != pre && target != pre->prev) {
        target = target->next;
        pre = pre->prev;
    }
    // unlinke this node
    element_t *entry = list_entry(target, element_t, list);
    list_del(target);

    // delete this entry
    q_release_element(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *node, *safe;
    bool last = false;  // check if the last one is duplicate

    list_for_each_safe (node, safe, head) {
        element_t *curr = list_entry(node, element_t, list);
        // check nodes except for the last to detect duplicates
        bool isDup = node->next != head &&
                     !strcmp(curr->value,
                             list_entry(node->next, element_t, list)->value);
        // if this one is dup or last one is dup
        if (isDup || last) {
            list_del(node);
            q_release_element(curr);
        }
        // record if the prev is dup
        last = isDup;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    // using list_move_tail to swap two nodes
    struct list_head *node = head->next;
    for (; node != head && node->next != head; node = node->next)
        list_move_tail(node->next, node);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k < 2)
        return;
    struct list_head *node, *safe, *tmp = head;
    LIST_HEAD(new_head);

    int count = 0;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&new_head, tmp, node);
            q_reverse(&new_head);
            // put the new head back to tmp
            list_splice_init(&new_head, tmp);
            count = 0;
            tmp = safe->prev;
        }
    }
}
/* Merge two lists together */
struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *newHead = NULL, **ptr = &newHead, **cur;
    while (l1 && l2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) >= 0)
            cur = &l2;
        else
            cur = &l1;
        *ptr = *cur;
        *cur = (*cur)->next;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) l1 | (uintptr_t) l2);
    return newHead;
}

/* Divide the linked list to half until it left with one element then merge */
struct list_head *merge_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return head;
    struct list_head *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    // unlink
    fast = slow->next;
    slow->next = NULL;
    // divide them further and merge
    struct list_head *left = merge_sort(head);
    struct list_head *right = merge_sort(fast);
    return merge(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    // unlink
    head->prev->next = NULL;
    head->next = merge_sort(head->next);
    // relink
    struct list_head *ptr = head;
    while (ptr->next) {
        ptr->next->prev = ptr;
        ptr = ptr->next;
    }
    ptr->next = head;
    head->prev = ptr;
    return;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int len = 0;
    struct list_head *curr = head->prev;
    while (curr != head && curr->prev != head) {
        element_t *target = list_entry(curr, element_t, list);
        element_t *prev = list_entry(curr->prev, element_t, list);
        ++len;
        // if curr is larger than prev -> delete prev
        if (strcmp(target->value, prev->value) > 0) {
            list_del(&prev->list);
            q_release_element(prev);
            len--;
        } else {
            curr = curr->prev;
        }
    }
    return len + 1;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *first = container_of(head->next, queue_contex_t, chain);
    // if there's only one list in it
    if (head->next == head->prev)
        return first->size;

    // traverse the queue
    struct list_head *cur = head->next->next;
    while (cur != head) {
        queue_contex_t *nextQ = container_of(cur, queue_contex_t, chain);
        // add nextQ to the beginning of the first
        list_splice_init(nextQ->q, first->q);
        nextQ->size = 0;
        cur = cur->next;
    }

    q_sort(first->q);
    first->size = q_size(first->q);

    return first->size;
}

