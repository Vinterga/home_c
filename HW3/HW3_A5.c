#include <stdio.h>
#include <stdlib.h>

int main()
{
    float a, b, c, mean;

    scanf("%f", &a);
    scanf("%f", &b);
    scanf("%f", &c);

    mean = (a + b + c)/3;

    printf("%.2f", mean);

    return 0;
}
