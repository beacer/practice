/* fb2-4 text concordance */
%option noyywrap nodefault yylineno case-insensitive

/* the symbol table */
%{
    struct ref {
        struct ref *next;
        char *filename;
        int flags;
        int lineno;
    };

    struct symbol {
        char *name;
        struct ref *reflist;
    };

    /* simple syntab of fixed size */
    #define NHASH   9997
    struct symbol symtab[NHASH];

    struct symbol *lookup(char *);
    void addref(int, char *, char *, int);

    char *curfilename;
%}

%%
/* rules for concordance generator */

/* skip common words */
a |
an |
and |
are |
as |
at |
be |
but |
for |
in |
is |
it |
of |
on |
or |
that |
the |
this |
to      /* ignore */

[a-z]+(\'(s|t))?        { addref(yylineno, curfilename, yytext, 0); }
.|\n                    /* ignore */
%%

static unsigned
symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned int c;

    while (c = *sym++)
        hash = hash * 9 ^ c;
    return hash;
}

struct symbol *
lookup(char *sym)
{
    struct symbol   *sp = &symtab[symhash(sym) % NHASH];
    int scount = NHASH;

    while (--scount >= 0) {
        if (sp->name && !strcmp(sp->name, sym))
            return sp;

        if (!sp->name) {
            sp->name = strdup(sym);
            sp->reflist = 0;
            return sp;
        }

        if (++sp >= symtab + NHASH)
            sp = symtab;
    }

    fputs("symbol table overflow\n", stderr);
    abort();
}

void addref(int lineno, char *filename, char *word, int flags)
{
    struct ref *r;
    struct symbol *sp = lookup(word);

    /* don't do dups of same line an file */
    if (sp->reflist && sp->reflist->lineno == lineno
            && sp->reflist->filename == filename)
        return;

    r = malloc(sizeof(struct ref));
    if (!r) {
        fputs("out of space\n", stderr);
        abort();
    }

    r->next = sp->reflist;
    r->filename = filename;
    r->lineno = lineno;
    r->flags = flags;
    sp->reflist = r;
}

main(int argc, char *argv)
{
    int i;

    if (argc < 2) {
        curfilename = "(stdin)";
        yylineno = 1;
        yylex();
    } else {
        for (i = 1; i < argc; i++) {
            FILE *f = fopen(argv[i], "r");
            if (!f) {
                perror(argv[i]);
                return 1;
            }
        }

        curfilename = argv[i];

        yyrestart(f);
        yylineno = 1;
        yylex();
        fclose(F);
    }

    printrefs();
}
