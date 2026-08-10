#include <stdio.h>

int main(int ac, char **av)
{
    for (int i = 1; i < ac; i++)
    {
        remove(av[i]);
    }

    return 0;
}