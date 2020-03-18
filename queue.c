#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q) {
        list_ele_t *newh;
        while (q->head) {
            newh = q->head->next;
            free(q->head->value);
            free(q->head);
            q->head = newh;
        }
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    if (!q) {
        return false;
    }
    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }
    newh->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newh->value) {
        free(newh);
        return false;
    }
    memset(newh->value, 0, strlen(s) + 1);
    snprintf(newh->value, strlen(s) + 1, "%s", s);

    newh->next = q->head;
    q->head = newh;
    q->size++;

    if (q->size == 1) {
        q->tail = newh;
    }
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }
    list_ele_t *newt;
    newt = malloc(sizeof(list_ele_t));

    if (!newt) {
        return false;
    }
    newt->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newt->value) {
        free(newt);
        return false;
    }
    memset(newt->value, 0, strlen(s) + 1);
    snprintf(newt->value, strlen(s) + 1, "%s", s);

    newt->next = NULL;
    q->size++;

    if (q->size == 1) {
        q->tail = newt;
        q->head = q->tail;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || q->size == 0) {
        return false;
    }
    list_ele_t *rmh = q->head;
    q->head = q->head->next;
    bool ret = false;

    if (sp) {
        strncpy(sp, rmh->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
        ret = true;
    }

    free(rmh->value);
    free(rmh);
    q->size--;
    return ret;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size == 0 || q->size == 1) {
        return;
    }
    list_ele_t *tmp = q->head->next;
    q->tail->next = q->head;
    while (tmp != q->tail) {
        q->head->next = tmp->next;
        tmp->next = q->tail->next;
        q->tail->next = tmp;
        tmp = q->head->next;
    }
    q->tail = q->head;
    q->tail->next = NULL;
    q->head = tmp;
    return;
}

list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    if (!l1) {
        return l2;
    }
    if (!l2) {
        return l1;
    }

    list_ele_t *head = NULL;
    list_ele_t *tmp = NULL;

    if (strcmp(l1->value, l2->value) <= 0) {
        head = l1;
        l1 = l1->next;
    } else {
        head = l2;
        l2 = l2->next;
    }

    tmp = head;
    while (l1 && l2) {
        if (strcmp(l1->value, l2->value) <= 0) {
            tmp->next = l1;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2 = l2->next;
        }
        tmp = tmp->next;
    }

    while (l1 || l2) {
        if (l1) {
            tmp->next = l1;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2 = l2->next;
        }
        tmp = tmp->next;
    }
    return head;
}

list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next) {
        return head;
    }
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow->next;
    slow->next = NULL;

    list_ele_t *l1 = merge_sort(fast);
    list_ele_t *l2 = merge_sort(head);
    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head || !q->head->next) {
        return;
    }
    q->head = merge_sort(q->head);
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
