#include <unistd.h>

void put_char(char c)
{
    write(STDOUT_FILENO, &c, 1);
}