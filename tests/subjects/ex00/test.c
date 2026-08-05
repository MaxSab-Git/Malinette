#include <unistd.h>

int main(int ac, char **av)
{
    if (ac == 2)
    {
        write(STDOUT_FILENO, av[1], 1);
    }
    write(STDOUT_FILENO, "\n", 1);

    return 0;
}