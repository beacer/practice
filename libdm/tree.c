#include "tree.h"

struct tree_node *tree_new(void)
{
    struct tree_node *node;

    if ((node = malloc(sizeof(struct tree_node))) == NULL)
        return NULL;

    node->parent = NULL;
    INIT_LIST_HEAD(&node->list);
    INIT_LIST_HEAD(&node->children);
    return node;
}

int tree_free(struct tree_node *node)
{
    if (!node)
        return -1;

    if (!list_empty(&node->children))
        return -1;

    list_del(&node->list);
    free(node);
    return 0;
}

/*
 * @where: add before or after the @sibling node, 
 *   if @sibling is NULL, add as first or last node;
 * @sibling: optional, see @where.
 * @node: node to add
 */
int tree_insert(struct tree_node *parent, int where, 
        struct tree_node *sibling, struct tree_node *node)
{
    if (!parent || !node)
        return -1;

    /* as a child of other tree ? */
    if (list_empty(&node->list))
        list_del(&node->list); /* remove from that tree */

    switch (where) {
    case TREE_ADD_BEFORE:
        if (sibling && sibling->parent == parent && !list_empty(sibling->list)) {
            __list_add(&node->list, &sibling->list.prev, &sibling.list);
        } else {
            list_add(&node->list, &parent->children)
        }
        break;
    case TREE_ADD_AFTER:
        if (sibling && sibling->parent == parent && !list_empty(sibling->list)) {
            __list_add(&node->list, &sibling->list, &sibling->list.next);
        } else {
            list_add_tail(&node->list, &parent->children)
        }
        break;
    default:
        return -1;
    }

    node->parent = parent;
    return 0;
}

int tree_remove(struct tree_node *node)
{
    if (!node || !list_empty(node->children))
        return -1;

    list_del(&node->list);
    INIT_LIST_HEAD(&node->list);
    node->parent = NULL;
    return 0;
}

struct tree_node * 
tree_lookup(struct tree_node *from, tree_match_cb_t match, void *priv)
{
    struct tree_node *child, *node;

    if (!from || !cb)
        return NULL;

    if (match(from, priv))
        return from;

    list_for_each_entry(child, &from->children, list) {
        if ((node = tree_lookup(child, match, priv)) != NULL)
            return node;
    }

    return NULL;
}
