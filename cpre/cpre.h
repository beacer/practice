/*
 * CPRE: C Encapsulated Perl Regex.
 *
 *   It's just an re-encapsulation of the PCRE
 *   (Perl Compatible Regular Expressions) lib, 
 *   aims to make it more easy to use regex related 
 *   operation like Perl in C.
 */
#ifndef __CPRE_H__
#define __CPRE_H__

/* see re_capstr()'s "index" parameter */
enum {
	CPRE_MAXIDX		= -1,
	CPRE_MAXMATCH	= -2,
};

/* errcode of CPRE */
typedef enum {
    CESUCCESS       = 0,
    CEOPTION        = -1,
    CEPARAM         = -2,
    CERECOMP        = -3,
    CEREEXEC        = -4,
    CENOMATCH       = -5,
    CEBUFSIZE       = -6,
    CENOMEM         = -7,
    CENOSPACE       = -8,
    CEINDEX         = -9,
} re_err_t;

/*
 * do m// operation.
 * "opt" can be "x", "s", "m", "i", "g", 
 * or any combinatio of them, like "xsi".
 * return CESUCCESS if success or other value on error.
 */
re_err_t re_match(const char *text, const char *regex, const char *opts);

/*
 * do s// operation.
 * "opt" can be "x", "s", "m", "i", "g", 
 * or any combinatio of them, like "xsi".
 * if the regex is matched successfully, 
 * "result" will be set by the text after substituted,
 * and need to be free if no more use.
 * return CESUCCESS if success or other value on error.
 */
re_err_t re_subs(const char *text, const char *regex, 
        const char *replace, const char *opt, char **result);

/*
 * get captured substring, like $1, $2, ... $+, $^N
 * index can be, 
 *   0: the string which whole expression match
 *   1, 2 ...: the N'th thring ...
 *   CPRE_MAXIDX: max index of substring captured
 *   CPRE_MAXMATCH: string captured by most right blacket
 * return CESUCCESS if success or other value on error.
 */
re_err_t re_capstr(int index, char *buf, int size);

/*
 * like @+, @-, get the start or end position 
 * of matched substing.
 * return CESUCCESS if success or other value on error.
 */
re_err_t re_vector(int begin[], int end[], int *nvect);

/*
 * get textual error message by error number.
 * return error message.
 */
const char *re_strerr(re_err_t errno);

#endif /* __CPRE_H__ */
