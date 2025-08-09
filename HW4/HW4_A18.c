#include <stdio.h>
#include <stdlib.h>

int main()
{
   int a, b;
    scanf("%d %d", &a, &b);

    if (a > b)
    {
        printf("Above\n");
    }
    else if (a < b)
    {
        printf("Less\n");
    }
    else
    {
        printf("Equal\n");
    }


return 0;

}