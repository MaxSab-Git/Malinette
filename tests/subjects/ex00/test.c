#include <stdio.h>

int main(int ac, char **av)
{
    if (ac == 2)
    {
        printf("%s", av[1]);
    }
    printf("\n");

    return 0;
}
