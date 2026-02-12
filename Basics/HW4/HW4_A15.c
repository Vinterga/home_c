#include <stdio.h>
#include <stdlib.h>

int main()
{
   float X1, Y1, X2, Y2, k, b;
   scanf("%f %f %f %f", &X1, &Y1, &X2, &Y2);

   k = (Y2 - Y1) / (X2 - X1); // Вычисляем угловой коэффициент k 
   b = Y1 - k*X1; // вывели b из уравнения прямой Y = k*X + b, взять можно любую из двух точек, я взяла первую


    printf("%.2f %.2f", k, b);


return 0;

}
