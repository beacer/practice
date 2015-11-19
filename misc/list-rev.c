/*
 * reverse a list
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

struct list_node {
    struct list_node *next;
    char *val;
};

struct list_head {
    struct list_node *first;
};

int list_init(struct list_head *list)
{
    assert(list);
    list->first = NULL;
    return 0;
}

int list_insert_tail(struct list_head *list, struct list_node *node)
{
    struct list_node *n;
    assert(list && node);

    for (n = list->first; n && n->next; n = n->next)
        ;

    if (!n)
        list->first = node;
    else
        n->next = node;
    node->next = NULL;
    return 0;
}

int list_reverse(struct list_head *list)
{
    struct list_node *n1, *n2, *t;
    assert(list);

    /* empty or has only one node */
    if (!list->first || !list->first->next)
        return 0;

    n1 = list->first, n2 = n1->next;
    while (n1 && n2) {
        t = n2->next;
        n2->next = n1;
        n1 = n2, n2 = t;
    }

    list->first->next = NULL; /* NOTE: donot forget !!*/
    list->first = n1;
    return 0;
}

int main(int argc, char *argv[])
{
    struct list_head list = {0};
    struct list_node *node;

    argc--, argv++;
    if (argc <= 0) {
        printf("usage ./a.out elem1 elem2 ...\n");
        exit(1);
    }

    list_init(&list);
    while (argc--) {
        node = malloc(sizeof(struct list_node));
        if (!node)
            exit(1);
        node->val = strdup(argv[0]);
        if (!node->val)
            exit(1);

        list_insert_tail(&list, node);
        argv++;
    }

    for (node = list.first; node; node = node->next) {
        printf("%s->", (char *)node->val);
    }
    printf("\n");

    list_reverse(&list);

    for (node = list.first; node; node = node->next) {
        printf("%s->", (char *)node->val);
    }
    printf("\n");

    exit(0);
}
