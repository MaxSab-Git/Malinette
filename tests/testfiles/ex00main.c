extern void put_char(char c);

int main(int ac, char **av)
{
    if (ac == 2)
    {
        for (int i = 0; av[1][i] != '\0'; i++)
        {
            put_char(av[1][i]);
        }
    }
    put_char('\n');

    return 0;
}
