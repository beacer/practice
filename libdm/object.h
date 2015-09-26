#ifndef __LIBDM_OBJECT_H__
#define __LIBDM_OBJECT_H__
#include "libdm.h"

void obj_free(struct dm_object *obj);
struct dm_object *obj_create(const struct dm_object *hint);

#endif /* __LIBDM_OBJECT_H__ */
