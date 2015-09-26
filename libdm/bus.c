#include "libubox/blobmsg.h"

#define DM_DEF_BUSADDR      "unix:/var/tmp/ubus.sock"

static struct ubus_context  *ubus;

/* all object share same bus (PRC) methods */

static const struct blobmsg_policy object_policy[] = {
};

static struct ubus_object dmb_object = {
    .name = "object",
};

int dm_bus_init(const char *addr)
{
    if (!addr)
        addr = DM_DEF_BUSADDR;

}
