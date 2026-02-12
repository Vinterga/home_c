/*А17) КАКОЕ ВРЕМЯ ГОДА
Ввести номер месяца и вывести название времени года.
Input format: Целое число от 1 до 12 - номер месяца.
Output format: Время года на английском: winter, spring, summer, autumn*/

#include <stdio.h>

int main(void)
{
    int month; // объявляем целочисленную переменную для хранения номера месяца

    //1) Вводим номер месяца
    scanf("%d", &month); //вводим число от 1 до 12

    //2)Определяем время года с помощью оператора swith
    switch(month) {

        //зимние месяцы
        case 12:
        case 1:
        case 2:
        printf("winter\n");
        break; //выходим из switch

        //весенние месяцы
        case 3:
        case 4:
        case 5:
        printf("spring\n");
        break; //выходим из switch

    //летние месяцы
        case 6:
        case 7:
        case 8:
        printf("summer\n");
        break; //выходим из switch

    //осенние месяцы
        case 9:
        case 10:
        case 11:
        printf("autumn\n");
        break; //выходим из switch
    }


    return 0;
}