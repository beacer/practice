#include "list.h"

struct tree_node {
    struct tree_node    *parent;
    struct list_head    list;       /* node of sibling list */
    struct list_head    children;    /* list of children */
};

#define TREE_ROOT_INIT(name) { \
    NULL, \
    { &(name).list, &(name).list }, \
    { &(name).children, &(name).children } \
}

#define TREE_ROOT(name) \
	struct tree_node name = TREE_ROOT_INIT(name)

/* where to insert a node */
enum {
    TREE_ADD_BEFORE,
    TREE_ADD_AFTER,
};

typedef bool (*tree_match_cb_t)(struct tree_node *node, void *priv);

struct tree_node *tree_new(void);
int tree_free(struct tree_node *node);

/*
 * @where: add before or after the @sibling node, 
 *   if @sibling is NULL, add as first or last node;
 * @sibling: optional, see @where.
 * @node: node to add
 */
int tree_insert(struct tree_node *parent, int where, 
        struct tree_node *sibling, struct tree_node *node);

int tree_remove(struct tree_node *node);

struct tree_node *
tree_lookup(struct tree_node *from, tree_match_cb_t match, void *priv);
