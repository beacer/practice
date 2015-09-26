#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpre.h"

static void usage(void)
{
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "    ./ACTIONS [OPTION]... TEXT REGEX [REPLACEMENT]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "ACTIONS\n");
    fprintf(stderr, "    match     Print matched substring. \n");
    fprintf(stderr, "    replace   Replace matched part of TEXT with REPLACEMENT. \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "    -I index  If action is match, it indicate index for\n");
    fprintf(stderr, "              captured sub string to print. index can be:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "              0:    string whole regex matched.\n");
    fprintf(stderr, "              1..n: the n'th substring matched.\n");
    fprintf(stderr, "              -1:   for most right capture brackets.\n");
    fprintf(stderr, "              -2:   for max index of captured brackets.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -x        Ignore whitespaces and allow comments. \n");
    fprintf(stderr, "    -s        DOT match all charaters mode\n");
    fprintf(stderr, "    -m        Enhanced line-anchor mode.\n");
    fprintf(stderr, "    -i        Case insensitive.\n");
    fprintf(stderr, "    -g        Global match mode.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLES\n");
    fprintf(stderr, "    ./match -i $MAIL 'Subject: (.*)'\n");
    fprintf(stderr, "    ./match -i -I 1 $DATA '(\\d+)\\.'\n");
    fprintf(stderr, "    ./replace -g -m $html '^\\s*$' '<p>'\n");
}

static int is_match;
static int cap_idx = -1;
static char options[10] = {0};
static char *text;
static char *regex;
static char *replace;

static int parse_args(int argc, char *argv[])
{
    int opt;
    int need;
    char *prog_name;
    
    need = 3;
    while ((opt = getopt(argc, argv, "I:xsmig")) != -1) {
        switch (opt) {
        case 'I':
            need += 2;
            cap_idx = atoi(optarg);
            break;
        case 'x':
            need++;
            strncat(options, "x", sizeof(options) - 1 - strlen(options));
            break;
        case 's':
            need++;
            strncat(options, "s", sizeof(options) - 1 - strlen(options));
            break;
        case 'm':
            need++;
            strncat(options, "m", sizeof(options) - 1 - strlen(options));
            break;
        case 'i':
            need++;
            strncat(options, "i", sizeof(options) - 1 - strlen(options));
            break;
        case 'g':
            need++;
            strncat(options, "g", sizeof(options) - 1 - strlen(options));
            break;
        case '?':
            fprintf(stderr, "%s: unknow option: %c\n", argv[0], optopt);
            exit(1);
        }
    }

    /* if argv[0] contain "\/" it must not valid */
    if ((prog_name = strrchr(argv[0], '/')) != NULL)
        prog_name++;
    else 
        prog_name = argv[0];

    if (strcasecmp(prog_name, "match") == 0) {
        if (argc < need)
            return -1;

        is_match = 1;
        text = argv[argc - 2];
        regex = argv[argc - 1];
    } else if (strcasecmp(prog_name, "replace") == 0) {
        if (argc < ++need)
            return -1;

        is_match = 0;
        text = argv[argc - 3];
        regex = argv[argc - 2];
        replace = argv[argc - 1];
    } else {
        fprintf(stderr, "%s is not valid action\n", argv[0]);
        exit(1);
    }

//    fprintf(stderr, "-------  Arguments  -------\n");
//    fprintf(stderr, "Action: %s\n", is_match ? "match" : "replace");
//    fprintf(stderr, "Index: %d\n", cap_idx);
//    fprintf(stderr, "Options: %s\n", options);
//    fprintf(stderr, "Text: %s\n", text);
//    fprintf(stderr, "Regex: %s\n", regex);
//    fprintf(stderr, "Replace: %s\n", replace);
//    fprintf(stderr, "\n");

    return 0;
}

int main(int argc, char *argv[])
{
    re_err_t ret;
    char buf[1024];
    char *result;

    if (parse_args(argc, argv) != 0) {
        usage();
        exit(1);
    }

    //fprintf(stderr, "-------  Execute  -------\n");

    if (is_match) {
        if ((ret = re_match(text, regex, options)) != CESUCCESS) {
            fprintf(stderr, "Not match: %s\n", re_strerr(ret));
            exit(1);
        }

        if (cap_idx >= 0) {
            if ((ret = re_capstr(cap_idx, buf, sizeof(buf))) != CESUCCESS) {
                fprintf(stderr, "re_capstr(): %s\n", re_strerr(ret));
                exit(1);
            }

            printf("[%d]: %s\n", cap_idx, buf);
        } else {
            if ((ret = re_capstr(0, buf, sizeof(buf))) != CESUCCESS) {
                fprintf(stderr, "re_capstr(): %s\n", re_strerr(ret));
                exit(1);
            }

            printf("%s\n", buf);
        }
    } else {
        if ((ret = re_subs(text, regex, replace, options, &result)) != CESUCCESS) {
            fprintf(stderr, "Fail to replace: %s\n", re_strerr(ret));
            exit(1);
        }

        //printf("Replaced!\n");
        printf("%s\n", result);
        free(result);
    }

    exit(0);
}
