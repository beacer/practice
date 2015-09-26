#include <stdio.h>
#include <unistd.h>
#include <string.h>

void chomp(char *str)
{
    if (str == NULL)
        return;

    if (strlen(str) > 0 && str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';
}

int sys_cmdout(const char *cmd, char *out, int size)
{
    FILE *fp;

    if (cmd == NULL || out == NULL || size < 0)
        return -1;

    if ((fp = popen(cmd, "rb")) == NULL)
        return -1;

    if (fread(out, 1, size, fp) == 0) {
        if (ferror(fp)) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    chomp(out);
    return 0;
}
