// ОСНОВНАЯ ПРОГРАММА main.c

#include <stdio.h>      // для printf
#include <string.h>     // для strcmp
#include <stdlib.h>     // для exit
#include <math.h>       // для fabs, isnan
 
#include "functions.h"  // наши функции f1, f2, f3 (теперь компил-р узнает о существ-ии этих ф-ций)
#include "root.h"       // функция поиска корня
#include "integral.h"   // функция интеграла (пока заглушка)
#include "test.h"       // тесты
#include <locale.h>     // локаль

// Переменные для хранения состояния опций командной строки
static int print_abscissas = 0;   // если 1 – печатать абсциссы точек пересечения
static int print_iterations = 0;  // если 1 – печатать число итераций
static int run_tests = 0;         // если 1 – запустить тесты и выйти

// Функция для вывода справки (вызывается по ключу --help)
static void print_help(const char *progname) {
    printf("Использование: %s [ОПЦИИ]\n", progname);
    printf("ОПЦИИ:\n");
    printf("  --help               показать эту справку и выйти\n");
    printf("  --print-abscissas    напечатать абсциссы точек пересечения кривых\n");
    printf("  --print-iterations   напечатать число итераций при поиске корней\n");
    printf("  --test               запустить тесты и выйти\n");
}

int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "");

    //Разбираем арг-ты ком-ной строки
    for (int i = 1; i < argc; i++) {           // начинаем с i=1, т.к. argv[0] – имя программы
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;                         //после вывода справки заершаем программу
        }
        else if (strcmp(argv[i], "--print-abscissas") == 0) {
            print_abscissas = 1;
        }
        else if (strcmp(argv[i], "--print-iterations") == 0) {
            print_iterations = 1;
        }
        else if (strcmp(argv[i], "--test") == 0) {
            run_tests = 1;
        }
        else {
            // Неизвестный аргумент – показываем ошибку и справку
            printf("Неизвестный аргумент: %s\n", argv[i]);
            print_help(argv[0]);
            return 1;
        }
    }
    
     // Если запрошены тесты – выполняем их и завершаем программу
    if (run_tests) {
        test_root(); // тестирует root и integral 
        return 0;
    }
    
    // Если не запрошены тесты, то приступаем к основной задаче – вычислению площади.
    printf("Программа вычисления площади фигуры, ограниченной тремя кривыми:\n");
    printf("f1(x) = 0.6x + 3\n");
    printf("f2(x) = (x-2)^3 - 1\n");
    printf("f3(x) = 3/x\n\n");


////////* ДЕЙСТВИЕ 1: Находим точки пересечения кривых
/*     Для этого нам нужно решить три уравнения:
       f1(x) = f2(x)
       f1(x) = f3(x)
       f2(x) = f3(x)
       Каждое уравнение может иметь несколько корней, но по графику (его нужно построить заранее)
       мы знаем, что есть три точки пересечения, образующие замкнутую фигуру.
       Интервалы для поиска корней подберём приблизительно, построив графики на сторонних ресурсах типа маткада.
       Для наших функций выберем отрезки, на которых гарантированно есть корни.*/
       
       
    double eps_root = 1e-7; // Точность для поиска корней
    int iter;  // для сохранения числа итераций

    // Корень уравнения f1 = f2
    double x12 = root(f1, f2, 3.0, 4.0, eps_root, &iter);
    if (print_iterations) {
        if (isnan(x12)) 
            printf("Итераций для f1=f2: ошибка (нет корня на отрезке)\n");
        else
            printf("Итераций для f1=f2: %d\n", iter);
    }

    // Корень уравнения f1 = f3
    double x13 = root(f1, f3, 0.5, 2.0, eps_root, &iter);
    if (print_iterations) {
        if (isnan(x13))
            printf("Итераций для f1=f3: ошибка\n");
        else
            printf("Итераций для f1=f3: %d\n", iter);
    }

    // Корень уравнения f2 = f3
    double x23 = root(f2, f3, 2.5, 4.0, eps_root, &iter);
    if (print_iterations) {
        if (isnan(x23))
            printf("Итераций для f2=f3: ошибка\n");
        else
            printf("Итераций для f2=f3: %d\n", iter);
    }
    
     // Проверяем, все ли корни найдены
    if (isnan(x12) || isnan(x13) || isnan(x23)) {
        printf("Ошибка: не удалось найти все точки пересечения. Проверьте интервалы.\n");
        return 1;
    }

    // Если запрошена печать абсцисс – выводим их
    if (print_abscissas) {
        printf("Абсциссы точек пересечения:\n");
        printf("  f1 = f2  при x = %.3f\n", x12);
        printf("  f1 = f3  при x = %.3f\n", x13);
        printf("  f2 = f3  при x = %.30f\n", x23);
    }

////////// ДЕЙСТВИЕ 2: ВЫЧИСЛЕНИЕ ПЛОЩАДИ

    double eps_int = 1e-6;  // точность для интеграла

    // Площадь первой части: =(f1 - f2) от x13 до x23
    double area1 = integral(f1, x13, x23, eps_int) - integral(f2, x13, x23, eps_int);

    // Площадь второй части: =(f1 - f3) от x23 до x12
    double area2 = integral(f1, x23, x12, eps_int) - integral(f3, x23, x12, eps_int);

    double total_area = area1 + area2;

    printf("\nПлощадь фигуры: %.3f\n", total_area);

    return 0;
}