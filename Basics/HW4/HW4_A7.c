#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a, b;

    scanf("%d", &a);
    scanf("%d", &b);

    if (a < b) {
            printf("%d %d", a, b);
    }
        else {
            printf("%d %d", b, a);
    }

    return 0;
}