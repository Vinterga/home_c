#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a, b, c, sum, prod;
    scanf("%d", &a);
    scanf("%d", &b);
    scanf("%d", &c);
    sum = a + b + c;
    prod = a * b * c;
    printf("%d+%d+%d=%d\n", a, b, c, sum);
    printf("%d*%d*%d=%d", a, b, c, prod);
    return 0;
}
