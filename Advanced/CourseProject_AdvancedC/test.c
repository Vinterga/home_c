// Реализация test.c

#include "test.h"
#include "root.h"
#include "integral.h"
#include <stdio.h>
#include <math.h>

// Вспомогательная функция для теста корней: левая часть уравнения x^2
static double test_f(double x) {
    return x * x;
}

// Вспомогательная функция для теста корней: правая часть уравнения 2
static double test_g(double x) {
    (void)x;         
    return 2.0;
}

// Тестовые функции для интеграла
// Для каждой вычисляем интеграл нашей функцией и сравниваем с точным значением
static double const_func(double x) {
    (void)x;
    return 2.0;                // константа 2
}

static double linear_func(double x) {
    return x;                   // линейная f(x)=x
}

static double square_func(double x) {
    return x * x;               // квадратичная f(x)=x^2
}

// Тестирование integral
static void test_integral(void) {
    printf("\n=== Тестирование функции integral ===\n");

    // Тест 1: константа 2 на отрезке [0, 2] -> площадь = 4
    double eps = 1e-7;
    double res = integral(const_func, 0.0, 2.0, eps);
    printf("integral(2) on [0,2] = %.10f, ожидается 4.0, разница = %.2e\n",
           res, fabs(res - 4.0));

    // Тест 2: f(x)=x на [0, 1] -> площадь = 0.5
    res = integral(linear_func, 0.0, 1.0, eps);
    printf("integral(x) on [0,1] = %.10f, ожидается 0.5, разница = %.2e\n",
           res, fabs(res - 0.5));

    // Тест 3: f(x)=x^2 на [0, 1] -> площадь = 1/3 = 0.3333333333
    res = integral(square_func, 0.0, 1.0, eps);
    printf("integral(x^2) on [0,1] = %.10f, ожидается 0.3333333333, разница = %.2e\n",
           res, fabs(res - 1.0/3.0));

    printf("=== Тест integral завершён ===\n\n");
}

// Тест нахождения корней
void test_root(void) {
    printf("=== Тестирование функции root ===\n");
    printf("Решаем уравнение x^2 = 2 на отрезке [1, 2]\n");
    printf("Ожидаемый корень: sqrt(2) = 1.4142135623730951\n");

    double a = 1.0, b = 2.0;
    double eps = 1e-10;
    int iterations;

    double x = root(test_f, test_g, a, b, eps, &iterations);

    printf("Найденный корень: %.15f\n", x);
    printf("Число итераций: %d\n", iterations);
    printf("Разность с точным значением: %.2e\n", fabs(x - sqrt(2.0))); //в экспон. формате, чтобы сразу видеть точность
    printf("=== Тест root завершён ===\n\n");

    
    test_integral(); // Теперь вызовем тест интеграла
}