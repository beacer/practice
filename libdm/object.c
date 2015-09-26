#include "list.h"
#include "tree.h"
#include "object.h"

/* duplicate the param for @src, but clean the values */
static int param_copy(struct dm_param *dst, const struct dm_param *src)
{
    assert(src != NULL && dst != NULL);

    dst->flags = src->flags;
    dst->type = src->type;

    if ((dst->name = strdup(src->name)) == NULL)
        return EDM_NOMEM;

    switch (dst->type) {
    case DM_PARAM_BOOL:
        dst->len = sizeof(dst->v_bool);
        dst->v_bool = src->v_bool;
        break;
    case DM_PARAM_INT32:
        dst->len = sizeof(dst->v_int32);
        dst->v_int32 = src->v_int32;
        break;
    case DM_PARAM_UINT32:
        dst->len = sizeof(dst->v_uint32);
        dst->v_uint32 = src->v_uint32;
        break;
    case DM_PARAM_STR:
        dst->len = src->len;
        if (src->len && src->v_str) {
            dst->v_str = strdup(src->v_str);
            if (!dst->v_str) {
                free(dst->name);
                return EDM_NOMEM;
            }
        }
        break;
    case DM_PARAM_BIN:
        dst->len = src->len;
        if (src->len && src->v_bin) {
            dst->v_bin = malloc(src->len);
            if (!dst->v_bin) {
                free(dst->name);
                return EDM_NOMEM;
            }
            memcpy(dst->v_bin, src->v_bin, dst->len);
        }
        break;
    default:
        return EDM_ARGUMENT;
    }

    return EDM_OK;
}

static int param_clear(struct dm_param *param)
{
    assert(param == NULL);

    if (param->name)
        free(param->name);

    if (param->type == DM_PARAM_STR)
        free(param->v_str);
    else if (param->type == DM_PARAM_BIN)
        free(param->v_bin);

    return EDM_OK;
}

void obj_free(struct dm_object *obj)
{
    assert(obj != NULL);

    if (obj->params) {
        for (i = 0; i < obj->nparam; i++)
            param_clear(&obj->params[i]);
        free(obj->params);
    }

    if (obj->path)
        free(obj->path);

    free(obj);
}

struct dm_object *obj_create(const struct dm_object *hint)
{
    struct dm_object *obj;

    if ((obj = calloc(1, sizeof(struct dm_object))) == NULL)
        return EDM_NOMEM;
    INIT_HLIST_NODE(&obj->hlist);
    INIT_TREE_NODE(&obj->tree);

    if (!hint)
        return obj;

    if ((obj->path = strdup(hint->path)) == NULL) {
        free(obj);
        return EDM_NOMEM;
    }

    obj->flags = hint->flags;
    obj->nparam = hint->nparam;
    obj->get_params = hint->get_params;
    obj->set_params = hint->set_params;
    obj->add_ins = hint->add_ins;
    obj->del_ins = hint->del_ins;

    if ((obj->params = malloc(obj->nparam * sizeof(struct dm_param))) == NULL) {
        obj_free(obj);
        return NULL;
    }

    for (i = 0; i < obj->nparam; i++) {
        if (param_copy(&obj->params[i], &hint->params[i]) != EDM_OK) {
            obj_free(obj);
            return NULL;
        }
    }

    return obj;
}
