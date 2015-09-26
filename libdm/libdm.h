#ifndef __LIBDM_H__
#define __LIBDM_H__

#include <stdint.h>
#include <stdbool.h>
#include <list.h>
#include <signal.h>

#define DM_PATH_SZ          257
#define DM_ACCE_TIMO        3000  // # ms

#ifndef NELELM
#define NELELM(a)           (sizeof(a) / sizeof((a)[0]))
#endif

enum {
    EDM_OK              = 0,
    EDM_ARGUMENT        = -1,
    EDM_NOMEM           = -2,
    EDM_CONFIG          = -3,
    EDM_TIMEOUT         = -4,
};

enum {
    DM_PARAM_RW         = 0x00000001,
};

struct dm_param {
    char                *name;
    int                 flags;

    dm_type_t           type;
    size_t              len;
    union {
        bool            boolean;
        int32_t         int32;
        uint32_t        uint32;
        char            *string;
        unsigned char   *binary;
    } val;

#define v_bool          val.boolean
#define v_int32         val.int32
#define v_uint32        val.uint32
#define v_str           val.string
#define v_bin           val.binary
};

/**
 * tree type of objects:
 * 1. simple object (sigle instance object)
 *    "Device.DeviceInfo."
 *    "Device.IP."
 * 2. multi-instance object (basic object, place holder)
 *    "Device.IP.Interface."
 * 3. instance of multi-instance object.
 *    "Device.IP.Interface.1."
 *
 * no need a flag to check if object is 'writable', check obj.add_obj
 */
enum {
    DM_OBJ_MULINS   =   0x00000001, /* object may have multiple instaces */
    DM_OBJ_INS      =   0x00000002, /* instance object */
};

struct dm_object {
    char                *path;
    int                 flags;
    int                 nparam;
    struct dm_param     *params;

    /* we use group operation so that access can be atomic 
     * for multiple parameters in same object */
    int (*get_params)(const struct dm_object *obj, const char *names, 
            dm_param_t params[], int cnt);
    int (*set_params)(const struct dm_object *obj, dm_param_t params[], int cnt);

    /* only "writable" multi-ins object */
    int (*add_ins)(struct dm_object *obj);
    int (*del_ins)(struct dm_object *obj, int ins);

    /*
     * internal use
     */
    int                 ins;    /* DM_OBJ_INS only */
    int                 next_ins;

    /* layout */
    struct hlist_head   hlist;
    struct tree_node    node;   /* for group operation */
};

int dm_init(const char *bus);
int dm_term(void);

/* register DM object/param definitions */
int dm_register(struct dm_object *objs, int nobj);
int dm_unregister(struct dm_object *objs, int nobj);

/**
 * Local and Remote DM access API.
 * these APIs are aync for local DM and wait DM_ACCESS_TIMO sec for remote DM
 * we do not support timeout version, until we have enough reason.
 */

/* if return success, it depend on caller to free @names */
int dm_get_names(const char *path, bool recursive, char (**names)[DM_PATH_SZ], int *cnt);

/* if return success, it depend on caller to free @names */
int dm_get_params(const char **paths, int npath, struct dm_param **params, int *cnt);

int dm_set_params(const struct dm_param *params, int cnt, int commit);
int dm_add_obj(const char *obj, int *ins);
int dm_del_obj(const char *obj, int ins);

typedef int (*get_names_cb_t)(void *priv, char (**names)[DM_PATH_SZ], int *cnt);
typedef int (*get_param_cb_t)(void *priv, dm_param_t **params, int *cnt);
typedef int (*async_cb_t)(void *priv, int result);
typedef int (*get_param_cb_t)(void *priv, dm_param_t **params, int *cnt);
typedef int (*nofity_cb_t)(void *priv, int event, const char *msg); // XXX?

/* Async version */
int dm_get_names_async(const char *path, bool recursive, get_names_cb_t cb, void *priv);
int dm_get_params_async(const char **paths, int npath, get_param_cb_t cb, void *priv);
int dm_set_params_async(const dm_param_t *params, int cnt, int commit, async_cb_t cb, void *priv);
int dm_add_obj_async(const char *obj, int *ins, async_cb_t cb, void *priv);
int dm_del_obj_async(const char *obj, int ins, async_cb_t cb, void *priv);

/* subscriber notify object events */
int dm_subsrcibe(const char *path, notify_cb_t cb, void *priv);
int dm_unsubsrcibe(const char *path);

/*
 * enhanced lookup.
 * prefix lookup with level support as well as wildcard lookup.
 * __NOTE__: if return success, it depend on caller to free() @names.
 */
enum {
    COND_RECURSIVE          = 0x00010000,   // until leaf node (paramters)
    COND_EXACTLY            = 0x00020000,
    COND_CHILDREN           = 0x00040000,   // same as level == 1

    COND_INSTANCE           = 0x00080000,   // only instances of an multi-ins object
    COND_PARAM              = 0x00100000,   // only params
    COND_WILDCARD           = 0x00200000,   // same with '*' in @path

#define COND_LEVEL_MASK     = 0x0000FFFF;
#define COND_GET_LEVEL(cond)    ((cond) & COND_LEVEL_MASK)
};

int dm_lookup(const char *path, int cond, char (**names)[DM_PATH_SZ], int *cnt);

#endif /* __LIBDM_H__ */
