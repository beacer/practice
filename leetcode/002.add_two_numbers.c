/*
 * https://leetcode.com/problems/add-two-numbers/
 *
 * You are given two linked lists representing two non-negative numbers. 
 * The digits are stored in reverse order and each of their nodes contain 
 * a single digit. Add the two numbers and return it as a linked list.
 *
 * Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
 * Output: 7 -> 0 -> 8
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

static const int radix = 10;

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2)
{
    struct ListNode *result = NULL, *last, *node;
    int carry = 0;

    for (; l1 || l2 || carry;  /* XXX: this is the **key** */
            l1 = (l1 ? l1->next : NULL), l2 = (l2 ? l2->next : NULL))
    {
        node = malloc(sizeof(struct ListNode));
        if (!node)
            goto cleanup;
        node->val = 0;
        node->next = NULL;

        // TODO: check if x->val is [0-9)

        if (l1)
            node->val += l1->val;
        if (l2)
            node->val += l2->val;
        if (carry) {
            node->val += carry;
            carry = 0;
        }

        /* may added carry-over or not */
        if (node->val >= radix) {
            carry = 1;
            node->val %= radix;
        }

        /* insert to list */
        if (!result)
            result = node;
        else
            last->next = node;
        last = node;
    }

    return result;

cleanup:
    /* TODO: */
    return NULL;
}

static struct ListNode *list_load(int num)
{
    struct ListNode *list = NULL, *node, *last;

    if (num < 0)
        return NULL;

    while (num) {
        node = malloc(sizeof(struct ListNode));
        if (!node)
            goto errout;
        node->val = num % radix;
        node->next = NULL;
        num /= radix;

        if (!list)
            list = node;
        else
            last->next = node;
        last = node;
    }

    return list;

errout:
    // TODO: clean up list
    return NULL;
}

static int list_unload(struct ListNode *list)
{
    int num = 0, multi = 1;

    for (; list; list = list->next) {
        num += list->val * multi;
        multi *= radix;
    }

    return num;
}

int main(int argc, char *argv[])
{
    struct ListNode *l1, *l2, *lsum;
    int num;

    if (argc != 3) {
        fprintf(stderr, "usage: ./a.out <number> <number>\n");
        exit(1);
    }

    l1 = list_load(atoi(argv[1]));
    l2 = list_load(atoi(argv[2]));

    lsum = addTwoNumbers(l1, l2);
    if (!lsum) {
        fprintf(stderr, "fail to add tow numbers!\n");
        exit(1);
    }

    num = list_unload(lsum);
    printf("result: %d\n", num);

    // TODO clean up @l1, @l2, @lsum;
    exit(0);
}
