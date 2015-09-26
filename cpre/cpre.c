/*
 * CPRE: C Encapsulated Perl Regex.
 *
 *   It's just an re-encapsulation of the PCRE
 *   (Perl Compatible Regular Expressions) lib, 
 *   aims to make it more easy to use regex related 
 *   operation like Perl in C.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pcre.h"
#include "cpre.h"

/* if OVECT_SIZE is 60 then 40 (2/3) is usefull for user, 
 * so use 19 pair of brackets at most (1 pair for whole regex) */
#define OVECT_SIZE      60

#ifndef NELEMS
#define NELEMS(arr)     (sizeof(arr) / sizeof((arr)[0]))
#endif

struct err_val {
    re_err_t errno;
    const char *errtext;
};

struct err_val err_map[] = {
    {CESUCCESS,     "Success"},
    {CEOPTION,      "Invalid options"},
    {CEPARAM,       "Invalid parameters"},
    {CERECOMP,      "Regex compile error"},
    {CEREEXEC,      "Regex not match"},
    {CENOMATCH,     "Nothing match for this index"},
    {CEBUFSIZE,     "Buffer too small"},
    {CENOMEM,       "Not enough memory"},
    {CENOSPACE,     "Not enough internal space"},
    {CEINDEX,       "Invalid index"},
};

struct match_info {
    const char *text;

    /* number of pairs be set in ovector.
     * if the OVECT_SIZE is 60, then 1 pair is for the 
     * whole pattern and 19 for captured brackets.
     * each pair is (START,END). if brackets(2) capture 
     * nothing, the pair for it will be (-1,-1).
     * if whole regex is not match pcre_exec() return 
     * a negtive number, and if ovector is too small 
     * then it return 0. (See "man pcreapi") */
    int nvect;
    int ovector[OVECT_SIZE];
};

/* save matched info for use later, 
 * but could not fullfill dynamic scope.
 * and it's not save in multi-thread. */
static struct match_info match_info;

/* return positive value for options,
 * 0 for no option, or -1 on error */
static int parse_opts(const char *opts)
{
    int options = 0;
    const char *ch;

    if (opts == NULL)
        return 0;

    for (ch = opts; *ch != '\0'; ch++) {
        switch(*ch) {
        case 'x': /* ignore whitespaces and allow comments */
            options |= PCRE_EXTENDED;
            break;
        case 's': /* dot match all chars */
            options |= PCRE_DOTALL;
            break;
        case 'm': /* enhanced line-anchor */
            options |= PCRE_MULTILINE;
            break;
        case 'i': /* case insensitive */
            options |= PCRE_CASELESS;
            break;
        case 'g': /* global match */
            /* it is not implemented by pcre options,
             * but in this lib (not here). */
            break;
        case 'o': /* compile only once */
        case 'c': /* not clear posi of string */
        case 'e': /* execute Perl command */
            fprintf(stderr, "%s: /%c is not implement\n", 
                    __FUNCTION__, *ch);
            break;
        default:
            return -1;
        }
    }

    return options;
}

static void save_match_info(const char *text, int rc, int ovector[])
{
    if (match_info.text != NULL)
        free((char *)match_info.text);
    match_info.text = strdup(text);
    match_info.nvect = rc;
    memcpy(match_info.ovector, ovector, rc * 2 * sizeof(int));

    return;
}

/* return no-NULL for expanded replace text after process back-ref,
 * in this case caller need free it (if no more use later).
 * return NULL if no back-ref or error happend */
static char *
expand_backref(const char *text, const char *replace, int rc, int ovector[])
{
    const char *ch;
    int backnum, i;
    char *reptext;
    int start, len, olen;

    if (text == NULL || replace == NULL || rc <= 1 || ovector == NULL)
        return NULL;

    len = strlen(replace) + 1;
    for (i = 1; i < rc; i++) /* first pair is for whole regex */
        len += ovector[i * 2 + 1] - ovector[i * 2];

    if ((reptext = calloc(1, len)) == NULL) {
        fprintf(stderr, "%s: no memory\n", __FUNCTION__);
        return NULL;
    }

    for (ch = replace; *ch != '\0'; ch++) {
        if (*ch != '\\') {
            olen = strlen(reptext);
            memcpy(reptext + olen, ch, 1);
            reptext[olen + 1] = '\0';
            continue;
        }

        /* skip 2 continous backslash, 
         * "\x" is also no a back-ref,
         * "\01" is not a back-ref */
        if (ch[1] == '\\' 
                || !isdigit(ch[1])
                || (ch[1] == '0' && isdigit(ch[2]))) {
            memcpy(reptext + strlen(reptext), ch, 2);
            ch++;
            continue;
        }

        backnum = atoi(ch + 1);

        /* now it's a back-ref, but
         * whatever it a valid back-ref or not,
         * don't put it into expanded replacement. */
        do {
            ch++;
        } while (isdigit(ch[1]));

        /* invalid back-ref */
        if (backnum > rc - 1)
            continue;

        start = ovector[backnum * 2];
        len = ovector[backnum * 2 + 1] - ovector[backnum * 2];
        olen = strlen(reptext);
        memcpy(reptext + olen, text + start, len);
        reptext[olen + len] = '\0';
    }

    return reptext;
}

re_err_t re_match(const char *text, const char *regex, const char *opts)
{
    pcre *re;
    const char *error;
    int erroffset;
    int options;
    int rc;
    int ovector[OVECT_SIZE];
    int offset;

    if (text == NULL || regex == NULL)
        return CEPARAM;

    if ((options = parse_opts(opts)) < 0)
        return CEOPTION;

    if ((re = pcre_compile(regex, options, &error, &erroffset, NULL)) == NULL)
        return CERECOMP;

    if ((rc = pcre_exec(re, NULL, text, strlen(text), 0, 0, 
                    ovector, NELEMS(ovector))) < 0) {
        pcre_free(re);
        return CEREEXEC;
    } else if (rc == 0) {
        /* it's just a waring (not an error) but may 
         * loss captured substing for the small ovector */
        fprintf(stderr, "%s: warning: too many brackets used\n", __FUNCTION__);
        match_info.nvect = 0;
        pcre_free(re);
        return CESUCCESS; /* not an error */
    }

    /* the pattern is match and 
     * at least one pair of offset is set */
    save_match_info(text, rc, ovector);

    /* /g modifier */
    if (strchr(opts, 'g') == NULL) {
        if (ovector[1] == ovector[0])
            offset = ovector[1] + 1;
        else
            offset = ovector[1];

        if (offset <= strlen(text))
            re_match(text + offset, regex, opts);
    }

    pcre_free(re);
    return CESUCCESS;
}

re_err_t re_subs(const char *text, const char *regex, 
        const char *replace, const char *opts, char **result)
{
    int options;
    pcre *re;
    const char *error;
    int erroffset;
    int rc;
    int ovector[OVECT_SIZE];
    char *replace2;
    const char *rep_real;
    int posi;
    int len;
    char *result2;
    int offset;
    char *tmp;

    if (text == NULL || regex == NULL || replace == NULL || result == NULL)
        return CEPARAM;

    if ((options = parse_opts(opts)) < 0)
        return CEOPTION;

    if ((re = pcre_compile(regex, options, &error, &erroffset, NULL)) == NULL) {
        fprintf(stderr, "%s: pcre_compile: %s\n", __FUNCTION__, error);
        return CERECOMP;
    }

    if ((rc = pcre_exec(re, NULL, text, strlen(text), 0, 0,
                    ovector, NELEMS(ovector))) < 0) {
        pcre_free(re);
        return CEREEXEC;
    } else if (rc == 0) {
        pcre_free(re);
        return CENOSPACE;
    }

    /* process back-reference with replace text */
    if ((replace2 = expand_backref(text, replace, rc, ovector)) != NULL)
        rep_real = replace2;
    else
        rep_real = replace;

    len = strlen(text) - (ovector[1] - ovector[0]) + strlen(rep_real) + 1;
    if ((*result = calloc(1, len)) == NULL) {
        if (replace2 != NULL)
            free(replace2);
        pcre_free(re);
        return CENOMEM;
    }

    posi = 0;
    memcpy(*result + posi, text, ovector[0]);
    posi += ovector[0];
    memcpy(*result + posi, rep_real, strlen(rep_real));
    posi += strlen(rep_real);
    memcpy(*result + posi, text + ovector[1], strlen(text) - ovector[1]);

    if (replace2 != NULL)
        free(replace2);

    /* substitute successfully */
    save_match_info(text, rc, ovector);

    /* /g modifier */
    if (strchr(opts, 'g') != NULL) {
        if (ovector[1] == ovector[0]) {
            offset = ovector[1] + 1;
        } else {
            offset = ovector[1];
        }

        if (offset <= strlen(text)) {
            if (re_subs(text + offset, regex, 
                        replace, opts, &result2) == CESUCCESS) {
                (*result)[strlen(*result) - strlen(text) + offset] = '\0';
                tmp = *result;
                if ((*result = realloc(*result, 
                               strlen(*result) + strlen(result2) + 1)) == NULL) {
                    free(tmp);
                    pcre_free(re);
                    return CENOMEM;
                }
                strcat(*result, result2);
                free(result2);
            }
        }
    }

    pcre_free(re);
    return CESUCCESS;
}

re_err_t re_capstr(int index, char *buf, int size)
{
    int len;

    if (buf == NULL || size <= 0)
        return CEPARAM;

    if (match_info.text == NULL || index >= match_info.nvect) {
        return CENOMATCH;
    }

    if (index == CPRE_MAXIDX) {
        index = match_info.nvect - 1;
    } else if (index == CPRE_MAXMATCH) {
        for (index = match_info.nvect - 1; index >= 0; index--) {
            if (match_info.ovector[index * 2] >= 0
                    || match_info.ovector[index * 2] >= 0)
                break;
        }
        if (index < 0)
            return CENOMATCH;
    } else if (index < 0) {
        return CEINDEX;
    }

    len = match_info.ovector[index * 2 + 1] \
          - match_info.ovector[index * 2];
    if (size <= len)
        return CEBUFSIZE;

    memcpy(buf, match_info.text + match_info.ovector[index * 2], len);
    buf[len] = '\0';

    return CESUCCESS;
}

re_err_t re_vector(int begin[], int end[], int *nvect)
{
    int i;

    if ((begin == NULL && end == NULL) || nvect == NULL || *nvect <= 0)
        return CEPARAM;

    if (*nvect < match_info.nvect)
        return CEBUFSIZE;

    *nvect = match_info.nvect;
    for (i = 0; i < match_info.nvect; i++) {
        if (begin != NULL)
            begin[i] = match_info.ovector[i + 2];

        if (end != NULL)
            end[i] = match_info.ovector[i + 2 + 1];
    }

    return CESUCCESS;
}

const char *re_strerr(re_err_t errno)
{
    int i;

    for (i = 0; i < NELEMS(err_map); i++) {
        if (errno == err_map[i].errno)
            return err_map[i].errtext;
    }

    return "Unknown error";
}
