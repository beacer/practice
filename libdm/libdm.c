#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "libdm.h"
#include "object.h"

#define DM_HASH_SZ          1024

static struct hlist_head    dm_obj_hash[DM_HASH_SZ];
static TREE_ROOT(dm_obj_tree);

static bool obj_tree_match(struct tree_node *node, void *priv)
{
    assert(node != NULL && priv != NULL);
    struct dm_object *obj = container_of(node, struct dm_object, node);
    char *path = priv;

    if (strcmp(obj->path, path) == 0)
        return true;
    else
        return false;
}

int dm_register(struct dm_object *objs, int nobj)
{
    struct dm_object *obj;
    struct hlist_head *head;
    char prefix[DM_PATH_SZ];
    struct tree_node *parent;

    if (!objs || nobj <= 0)
        return EDM_ARGUMENT;

    for (i = 0; i < nobj; i++) {
        if ((obj = obj_create(&objs[i])) == NULL)
            continue;

        /* add to global hash */
        head = &obj_hash[obj_hash(obj->path)];
        hlist_add_head(&obj->hlist, head);

        /* add to global tree */
        if (get_parent_path(obj->path, prefix) == NULL
                || (parent = tree_lookup_node(dm_obj_tree, obj_tree_match, prefix)) == NULL)
            parent = &dm_obj_tree;

        if (tree_insert_node(parent, TREE_ADD_AFTER, NULL, &obj->node) != 0)
            fprintf(stderr, "%s: fail to insert obj: %s\n", __FUNCTION__, obj->path);
    }

    return EDM_OK;
}

int dm_init(const char *bus)
{
    int err, i;

    for (i = 0; i < NELEMS(obj_hash); i++)
        INIT_HLIST_HEAD(&obj_hash[i]);

    if ((err = dm_bus_init(bus)) != EDM_OK) {
        fprintf(stderr, "%s: fail to connect bus: %s\n", __FUNCTION__, dm_strerror(err));
        return err;
    }

    return EDM_OK;
}

static bool is_path_param(const char *path)
{
}

int dm_get_names(const char *path, bool recursive, char (**names)[DM_PATH_SZ], int *cnt)
{
    int condition = recursive ? (COND_RECURSIVE : (COND_EXACTLY | COND_CHILDREN));

    return dm_lookup(path, condition, names, cnt);
}

int dm_get_params(const char **paths, int npath, struct dm_param **params, int *cnt)
{
}
