#include <stdio.h>
#include <xtables.h>

static void echo_tg_help(void)
{
	printf("ECHO takes no options\n\n");
}

static int echo_tg_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_target **target)
{
	return 0;
}

static void echo_tg_check(unsigned int flags)
{
}

static struct xtables_target echo_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "ECHO",
	.family        = NFPROTO_UNSPEC,
	.help          = echo_tg_help,
	.parse         = echo_tg_parse,
	.final_check   = echo_tg_check,
};

static __attribute__((constructor)) void echo_tg_ldr(void)
{
	xtables_register_target(&echo_tg_reg);
}
