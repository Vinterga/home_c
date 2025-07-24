#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a, b, c, sum;
    scanf("%d", &a);
    scanf("%d", &b);
    scanf("%d", &c);
    sum = a + b + c;
    printf("%d+%d+%d=%d", a, b, c, sum);
    return 0;
}
