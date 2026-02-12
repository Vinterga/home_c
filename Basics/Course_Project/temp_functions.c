// ПОДКЛЮЧЕНИЕ СТАНДАРТНЫХ БИБЛИОТЕК
#include <stdio.h>          // Для ф-ций ввода/вывода: printf, fprintf
#include <stdlib.h>         // Для работы с памятью: malloc, free, realloc
#include <string.h>         // Для работы со строками: strcspn
#include <ctype.h>          // Для проверки символов: isdigit
#include <limits.h>         // Для констант INT_MAX и INT_MIN
#include "temp_functions.h" // Моя либа со структурами и функциями

#ifndef MAX_DISPLAY_RECORDS
#define MAX_DISPLAY_RECORDS 5 //Максимальное кол-во записей для показа в предпросмотре
#endif // MAX_DISPLAY_RECORDS

//ФУНКЦИЯ ДЛЯ ВЫВОДА РАЗДЕЛИТЕЛЬНОЙ ЛИНИИ
void print_separator(int width) {
    for (int i = 0; i < width; i++) {
            printf("=");
    }
    printf("\n");
}

// ФУНКЦИЯ ВЫВОДА СПРАВКИ
void print_help(void) {

    // Заголовок программы
    printf("┌────────────────────────────────────────────────────────────┐\n");
    printf("│           ПРОГРАММА АНАЛИЗА ТЕМПЕРАТУРНЫХ ДАННЫХ           │\n");
    printf("└────────────────────────────────────────────────────────────┘\n\n");

    // Описание программы
    printf("НАЗНАЧЕНИЕ:\n");
    printf("  Анализ статистики температуры из CSV-файлов и вывод отчетов\n\n");

    // Список поддерживаемых ключей командной строки
    printf("ИСПОЛЬЗОВАНИЕ:\n");
    printf("  ./temp_stats [ОПЦИИ]\n\n");

    printf("ОПЦИИ:\n");
    printf("  -h              Вывести справку и завершить работу\n");
    printf("  -f ФАЙЛ.csv     Указать CSV-файл с данными для анализа\n");
    printf("  -m МЕСЯЦ        Вывести статистику только для указанного месяца (1-12)\n\n");

    // Примеры использования
    printf("ПРИМЕРЫ:\n");
    printf("  ./temp_stats -h\n");
    printf("  ./temp_stats -f temperature_big.csv\n");
    printf("  ./temp_stats -f temperature_small.csv -m 3\n");
    printf("  ./temp_stats -f temperature_small.csv -m 12\n\n");

    // Информация о файлах с данными
    printf("ФАЙЛЫ ДАННЫХ:\n");
    printf("  temperature_big.csv    - полные данные за год\n");
    printf("  temperature_small.csv  - тестовые данные с ошибками\n\n");

    // Информация о формате данных
    printf("ФОРМАТ CSV-ФАЙЛА:\n");
    printf("  YEAR;MONTH;DAY;HOUR;MINUTE;TEMPERATURE\n");
    printf("  dddd;mm;dd;hh;mm;temperature\n");
    printf("  2021;01;15;14;30;-5\n");
    printf("  2021;02;20;9;15;10\n\n");
}

    //ФУНКЦИЯ, ПРОВЕРЯЮЩАЯ, ЯВЛЯЕТСЯ ЛИ СТРОКА ЦЕЛЫМ ЧИСЛОМ  (проверяет, что все символы в строке - цифры)
    //ДОБАВИТЬ ОБРАБОТКУ ПРОБЕЛОВ!!!!!
    int is_integer(const char *str) {
        if (str == NULL || *str == '\0') { // Проверка пустой строки или NULL указателя
            return 0;                      // Не является целым числом
        }

        int i = 0; // Проверка каждого символа в строке

        // Пропускаем пробелы в начале
        while (str[i] == ' ' || str[i] == '\t') {
            i++;
        }
    
        // Если после пробелов ничего нет - не число
         if (str[i] == '\0') {
            return 0;
        }

        if (str[i] == '-') {      // Обработка отрицательных чисел
            i++;                  // Пропускаем знак минус, если он есть
            if (str[i] == '\0') { // Если после минуса ничего нет,
                return 0;         // то это не число - возвращаем 0
            }
        }

        for (; str[i] != '\0'; i++) { // Проверяем каждый символ до конца строки, игнорируя пробелы в конце
            if (str[i] == ' ' || str[i] == '\t') { // Пропускаем оставшиеся пробелы
                while (str[i] == ' ' || str[i] == '\t') { // Если встретили пробел - проверяем, что дальше только пробелы до конца строки
                    i++;
            }
            
            if (str[i] != '\0') { // Если после пробелов не конец строки - ошибка
                return 0;
            }
            break;
        }
        
        if (!isdigit((unsigned char)str[i])) {
            return 0; // Найден символ, не являющийся цифрой
        }
    }

    return 1; // Все супер, это целое число (возможно с пробелами)
}

    // ФУНКЦИЯ ДЛЯ УДАЛЕНИЯ ПРОБЕЛОВ ИЗ СТРОКИ
    void remove_spaces(char *str) {
        if (str == NULL) return;
        
        char *dst = str;
        char *src = str;
        
        // Копируем только непробельные символы
        while (*src) {
            if (*src != ' ' && *src != '\t') {
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0';
    }

    // ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ НАЗВАНИЯ МЕСЯЦА ПО ЕГО НОМЕРУ
    const char* get_month_name(int month) {
        static const char* month_names[] = { // Массив с названиями месяцев
            "???",        // Индекс 0 не используется
            "Январь        ",     // 1
            "Февраль       ",    // 2
            "Март          ",       // 3
            "Апрель        ",     // 4
            "Май           ",        // 5
            "Июнь          ",       // 6
            "Июль          ",       // 7
            "Август        ",     // 8
            "Сентябрь      ",   // 9
            "Октябрь       ",    // 10
            "Ноябрь        ",     // 11
            "Декабрь       ",    // 12
        };

        if (month >= 1 && month <= 12) { //Проверка корректности ввода номера месяца
            return month_names[month];
            } else {
                return month_names[0]; // Возвращаем "???" для некорректного номера
            }
        }

    // ФУНКЦИЯ ЧТЕНИЯ И ПАРСИНГА CSV ФАЙЛА
    int read_csv(const char* filename, TemperatureRecord **data, int *total_records) {

        FILE *file = fopen(filename, "r"); // Открываем файл для чтения (r - режим чтения текстового файла)

        if (file == NULL) {   // Если не удалось открыть файл,
            fprintf(stderr, "ОШИБКА: Не удалось открыть файл '%s'\n", filename);
            fprintf(stderr, "Проверьте правильность имени файла и путь\n");
            return -1;        // то возвращаем код ошибки
        }

        //Выделение памяти для буфера чтения строки
        char buffer[256];      // Буфер для хранения одной строки файла (максимум 255 символов)
        int line_num = 0;      // Счетчик строк для сообщений об ошибках
        int capacity = 100;    // Начальная емкость массивая записей
        int records_count = 0; // Счетчик успешно загруженных записей
        int error_count = 0;   //Счетчик ошибок при чтении файла

        //Выделение памяти для массива записей
        *data = malloc(capacity * sizeof(TemperatureRecord));
        if (*data == NULL) {    // Если не удалось выделить память,
            fprintf(stderr, "ОШИБКА: Не удалось выделить память для данных\n");
            fclose(file);       // то закрываем файл
            return -1;          // и возвращаем код ошибки -1
        }

        //Читаем первую строку для проверки заголовка
        if (fgets(buffer, sizeof(buffer), file) == NULL) { // Если файл пустой,
            fprintf(stderr, "ОШИБКА: Файл '%s' пустой или содержит только заголовок\n", filename); // то выводим ошибку
            fclose(file);
            free(*data);                                   // Освобождаем память перед выходом
            *data = NULL;
            return -1;
        }
        line_num++; //Увеличиваем счетчик строк (заголовок - строка 1)

        //Удаляем символ переноса строки из заголовка, если таковой имеется
        buffer[strcspn(buffer, "\r\n")] = '\0'; // strcspn ищет позицию символа \r или \n и заменяет его на конец строки

        //Проверка формата заголовка
        int has_header = 0;
        if (strcmp(buffer, "YEAR;MONTH;DAY;HOUR;MINUTE;TEMPERATURE") == 0) {
            has_header = 1;
            printf("Обнаружен стандартный заголовок - пропускаем\n");
        } else {
            // Если это не заголовок, то это первая строка данных
            // Возвращаемся к началу файла, чтобы прочитать ее как данные
            fseek(file, 0, SEEK_SET);
            line_num = 0; // Сбросим счетчик, т.к. начнем читать с начала
            printf("Файл не содержит стандартного заголовка - читаем как данные\n");
        }

        //Основной цикл чтения файла построчно
        printf("Начинаем чтение файла '%s'...\n", filename);
        
        while (fgets(buffer, sizeof(buffer), file) != NULL) { // Читаем строку в буфер
            line_num++;                                       // Увеличиваем счетчик строк для каждой новой строки
            buffer[strcspn(buffer, "\r\n")] = '\0';           // Удаляем символ переноса строки

            if (buffer[0] == '\0') { // Пропускаем пустые строки
                continue;            // Переходим к следующей строке
            }
            
            /*int has_header = 0; // Проверяем, является ли первая строка заголовком
            if (strcmp(buffer, "YEAR;MONTH;DAY;HOUR;MINUTE;TEMPERATURE") == 0) {
                has_header = 1;
                printf("Обнаружен стандартный заголовок - пропускаем\n");
            } else {
                // Если это не заголовок, то это первая строка данных
                // Возвращаемся к началу файла чтобы обработать эту строку как данные
                fseek(file, 0, SEEK_SET);
                line_num = 0;
                printf("Файл не содержит стандартного заголовка - читаем как данные\n");
            }*/

            //Разбиваем строку на поля по разделителю ';'
            char *token;              // Указатель на текующее поле
            char *rest = buffer;      // Ук-ль на остаток строки для разбора
            int field_count = 0;      // Счетчик полей в строке
            TemperatureRecord record; // Временная структура для разбора данных
            int valid_record = 1;     // Флаг валидности записи (1 = валидна, 0 = невалидна)

            // Парсинг полей строки
            for(field_count = 0; field_count < 6; field_count++) {
                token = strtok_r(rest, ";", &rest); // Извлекаем следующее поле (токен)

                // strtok_r - безопасная версия strtok для многопоточности
                // 1-ый вызов: передаем исходную строеу, последующие - NULL

                if (token == NULL) {  // Проверка, что поле не пустое
                    fprintf(stderr, "ОШИБКА в строке %d: Недостаточно полей (ожидалось 6, найдено %d)\n", line_num, field_count);
                    valid_record = 0; // Помечаем запись как невалидную
                    error_count++;    // Увеличиваем счетчик ошибок
                    break;            // Выходим из цикла разбора полей
                }

                // Удаляем пробелы из токена
                remove_spaces(token);

                if(!is_integer(token)) { // Проверка, что поле - целое число
                    fprintf(stderr, "ОШИБКА в строке %d: Поле %d ('%s') не является целым числом\n", line_num, field_count + 1, token);
                    valid_record = 0;    // Помечаем запись как невалидную
                    error_count++;       // Увеличиваем счетчик ошибок
                    break;               // Выходим из цикла разбора полей
                }

                int value = atoi(token); //Преобр-м строку в целое число и сохраняем в соотв-щее поле стр-ры

                switch (field_count) {
                    case 0: //ГОД
                        record.year = value; //Сохраняем год в структуру
                        if (value < 1000 || value > 9999) {
                            fprintf(stderr, "ОШИБКА в строке %d: Некорректный год %d (должен быть 4 цифры)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                        }
                        break;

                    case 1: //МЕСЯЦ
                        record.month = value; //Сохраняем месяц в структуру
                        if (value < 1 || value > 12) {
                        fprintf(stderr, "ОШИБКА в строке %d: Некорректный месяц %d (должен быть 1-12)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                        }
                        break;

                    case 2: //ДЕНЬ
                     record.day = value;
                    if (value < 1 || value > 31) {
                        fprintf(stderr, "ОШИБКА в строке %d: Некорректный день %d (должен быть 1-31)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                    }
                    break;

                case 3:  //ЧАСЫ
                    record.hour = value;
                    if (value < 0 || value > 23) {
                        fprintf(stderr, "ОШИБКА в строке %d: Некорректный час %d (должен быть 0-23)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                    }
                    break;

                case 4:  //МИНУТЫ
                    record.minute = value;
                    if (value < 0 || value > 59) {
                        fprintf(stderr, "ОШИБКА в строке %d: Некорректные минуты %d (должны быть 0-59)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                    }
                    break;

                case 5:  //ТЕМПЕРАТУРА
                    record.temperature = value;
                    if (value < -99 || value > 99) {
                        fprintf(stderr, "ОШИБКА в строке %d: Некорректная температура %d (должна быть -99..99)\n", line_num, value);
                        valid_record = 0;
                        error_count++;
                    }
                    break;
                }

                if (!valid_record) { // Если уже обнаружена ошибка, прекращаем проверку остальных полей
                break;
                }
            }

            if (valid_record && field_count != 6) { // Проверка количества полей
            fprintf(stderr, "ОШИБКА в строке %d: Неправильное количество полей (ожидалось 6)\n", line_num);
            valid_record = 0;
            error_count++;
            }

            if (valid_record) {                  // Если запись валидна, сохраняем ее в массив
                if (records_count >= capacity) { // Проверяем, нужно ли расширить массив
                    capacity *= 2;               // Удваиваем вместимость массива
                    TemperatureRecord *new_data = realloc(*data, capacity * sizeof(TemperatureRecord)); // realloc изменяет размер ранее выделенного блока памяти

                    if (new_data == NULL) { //Если не удалось перевыделить память
                        fprintf(stderr, "ОШИБКА: Не удалось расширить массив данных\n");
                        fclose(file);
                        free(*data);
                        *data = NULL;
                        *total_records = 0;
                        return -1;
                    }
                    *data = new_data; //Обновляем указатель на новый массив
                }

                (*data)[records_count] = record; //Сохраняем запись в массив
                records_count++;                 //Увеличиваем счетчик успешных записей
            }
        }

        if(!feof(file)) { // Проверяем, произошла ли ошибка чтения файла (кроме конца файла)
            // feof проверяет, достигнут ли конец файла
            // Если не конец файла, значит, прозошла ошибка чтения файла
            fprintf(stderr, "ОШИБКА: Произошла ошибка при чтении файла\n");
            fclose(file);
            free(*data);
            *data = NULL;
            *total_records = 0;
            return -1;
        }

        fclose(file); // Закрываем файл после успешного чтения

        printf("Чтение файла завершено:\n");                       // Выводим сводную информацию о результатах чтения
        printf("  - Всего строк обработано: %d\n", line_num - 1);  // минус заголовок
        printf("  - Успешно загружено записей: %d\n", records_count);
        printf("  - Найдено ошибок: %d\n", error_count);

        *total_records = records_count; //Сохр-м рез-ты в переменные, переданные по указателю

        if (records_count == 0) { //Если нет ни одной валидной записи, выводим предупреждение
        printf("ПРЕДУПРЕЖДЕНИЕ: В файле нет валидных записей для анализа\n");
        }

        return 0; //Успешное выполнение функции, ну наконец-то
    }

    //ОСНОВНАЯ ФУНКЦИЯ РАСЧЕТА И ВЫВОДА СТАТИСТИКИ
    void calculate_stats(TemperatureRecord *data, int total_records, int target_month) {
       if (total_records == 0) {
        printf("\nНет данных для анализа статистики.\n");
        return;
       }

       printf("\n"); // Вывод заголовка статистики
       printf("┌────────────────────────────────────────────────────────────┐\n");
       printf("│                   СТАТИСТИКА ТЕМПЕРАТУРЫ                   │\n");
       printf("└────────────────────────────────────────────────────────────┘\n\n");

       // Инициализация переменных для месячной статистики
       // Используем массивы для хранения статистики по каждому месяцу (индексы 1-12)
       float monthly_sum[13] = {0}; // Сумма температур по месяцам
       int monthly_count[13] = {0}; // Кол-во записей по месяцам
       int monthly_min[13];         // Минимальная температура по месяцам
       int monthly_max[13];         // Максимальная температура по месяцам

       // Инициализация переменных для годовой статистики
       float year_sum = 0;     // Общая сумма температур за год
       int year_min = INT_MAX; // Абсолютный min-м за год (иниц-м максимальным значением)
       int year_max = INT_MIN; // Абсолютный max-м за год (иниц-м минимальным значением)
       int year_count = 0;     // Общее кол-во записей за год

       //Инициализация массивов месячной статистики начальными значениями
       for(int i = 0; i <= 12; i++) {
           monthly_min[i] = INT_MAX; // Иниц-м большим значением
           monthly_max[i] = INT_MIN; // Иниц-м малым значением
       }

       //Основной цикл обработки всех записей и расчета статистики
       for (int i = 0; i < total_records; i++) {
        int month = data[i].month;      // Извлекаем месяц из текущей записи
        int temp = data[i].temperature; // Извлекаем темп-ру из текущей записи

           // Обновление месячной статистики
           if (month >= 1 && month <= 12) { // Проверяем корректность номера месяца
               monthly_sum[month] += temp;  // Добавляем температуру к сумме месяца
               monthly_count[month]++;      // Увеличиваем счетчик записей месяца

                //Обновление минимума месяца
                if (temp < monthly_min[month]) {
                    monthly_min[month] = temp;
                }

                //Обновление максимума месяца
                if (temp > monthly_max[month]) {
                    monthly_max[month] = temp;
                }
            }

            //Обновление годовой статистики
            year_sum += temp; //Добавляем темп-ру к годовой сумме
            year_count++;     //Увелич-м счетчик записей

            //Обновление абсол-го минимума за год
            if (temp < year_min) {
                year_min = temp;
            }

            //Обновление абсол-го минимума за год
            if (temp > year_max) {
                year_max = temp;
            }
       }

       //Вывод статистики в зависимости от режима (все месяца или конкретный месяц)
        if (target_month == 0) {
        // РЕЖИМ: Вывод статистики за все месяцы
        printf("\nМЕСЯЧНАЯ СТАТИСТИКА:\n");
        printf("┌──────┬────────────────┬───────────┬───────────┬───────────┐\n");
        printf("│Месяц │ Название       │  Средняя  │  Минимум  │  Максимум │\n");
        printf("├──────┼────────────────┼───────────┼───────────┼───────────┤\n");

        //Вывод статистики для каждого месяца
        for (int month = 1; month <= 12; month++) {
            if (monthly_count[month] > 0) {
                //Расчет сред. темп-ры для месяца
                float avg_temp = monthly_sum[month] / monthly_count[month];

                //Вывод строки со статистикой для месяца
                printf("│ %4d │ %-14s │ %7.2f°C │ %7d°C │ %7d°C │\n", month, get_month_name(month), avg_temp, monthly_min[month], monthly_max[month]);
            } else {
                // Если данных за месяц нет, выводим сообщение
                printf("│ %4d │ %-14s │   НЕТ    │  ДАННЫХ  │   НЕТ    │\n", month, get_month_name(month));
            }
        }

         printf("└──────┴────────────────┴───────────┴───────────┴───────────┘\n");

         //Вывод годовой статистики
         printf("\nГОДОВАЯ СТАТИСТИКА:\n");
         printf("┌────────────────────┬──────────┬──────────┐\n");
         printf("│ Показатель         │ Значение │ Единица  │\n");
         printf("├────────────────────┼──────────┼──────────┤\n");

        //Расчет среднегодовой температуры
        float year_avg = year_sum / year_count;

        //Вывод годовых показателей
        printf("│ Средняя за год     │ %7.2f  │    °C    │\n", year_avg);
        printf("│ Абс. минимум       │ %7d  │    °C    │\n", year_min);
        printf("│ Абс. максимум      │ %7d  │    °C    │\n", year_max);
        printf("│ Всего измерений    │ %7d  │  записей │\n", year_count);
        printf("└────────────────────┴──────────┴──────────┘\n");
       } else {
       //РЕЖИМ: Вывод статистики за конкретный месяц}
            int month = target_month;

            printf("\nСТАТИСТИКА ЗА %s (%d МЕСЯЦ):\n", get_month_name(month), month);
            printf("┌────────────────────┬──────────┬──────────┐\n");

            if (monthly_count[month] > 0) {
                //Расчет средней темп-ры для выбранного месяца
                float avg_temp = monthly_sum[month] / monthly_count[month];

                //Вывод статистики для выбранного месяца
                 printf("│ Показатель         │ Значение │ Единица  │\n");
                 printf("├────────────────────┼──────────┼──────────┤\n");
                 printf("│ Средняя температура│ %7.2f  │    °C    │\n", avg_temp);
                 printf("│ Минимальная        │ %7d  │    °C    │\n", monthly_min[month]);
                 printf("│ Максимальная       │ %7d  │    °C    │\n", monthly_max[month]);
                 printf("│ Кол-во измерений   │ %7d  │  записей │\n", monthly_count[month]);
            } else {
                printf("│ НЕТ ДАННЫХ ДЛЯ ВЫБРАННОГО МЕСЯЦА │\n");
}
printf("└────────────────────┴──────────┴──────────┘\n");

            //Дополнительно: вывод некоторых записей из выбранного месяца для наглядности
            if (monthly_count[month] > 0) {/*

                printf("\nПЕРВЫЕ 5 ЗАПИСЕЙ ЗА %s:\n", get_month_name(month));
                printf("┌──────────┬──────┬────┬────┬──────┬─────────────┐\n");
                printf("│   Дата   │Время │Темп│ №  │ Месяц│   Запись    │\n");
                printf("├──────────┼──────┼────┼────┼──────┼─────────────┤\n");

                int displayed = 0;
                for (int i = 0; i < total_records && displayed < 5; i++) {
                    if (data[i].month == month) {
                         printf("│ %04d-%02d-%02d │ %02d:%02d │ %3d°C│ %3d │  %2d  │ Пример     │\n",
                           data[i].year, data[i].month, data[i].day,
                           data[i].hour, data[i].minute, data[i].temperature,
                           displayed + 1, month);
                    displayed++;
                    }
                }
                printf("└──────────┴──────┴────┴────┴──────┴─────────────┘\n");
                }*/

                printf("\nПЕРВЫЕ %d ЗАПИСЕЙ ЗА %s:\n", MAX_DISPLAY_RECORDS, get_month_name(month));
                printf("┌────────────┬────────┬─────────────┐\n");
                printf("│    Дата    │ Время  │ Температура │\n");
                printf("├────────────┼────────┼─────────────┤\n");

                int displayed = 0;
                for (int i = 0; i < total_records && displayed < MAX_DISPLAY_RECORDS; i++) {
                    if (data[i].month == month) {
                        printf("│ %04d-%02d-%02d │ %02d:%02d  │    %3d°C    │\n",
                               data[i].year, data[i].month, data[i].day,
                               data[i].hour, data[i].minute, data[i].temperature);
                        displayed++;
                    }
                }
                printf("└────────────┴────────┴─────────────┘\n");
                printf("... и еще %d записей\n", monthly_count[month] - displayed);
            }
    }

    //Вывод доп. инф-ции
    printf("\nДОПОЛНИТЕЛЬНАЯ ИНФОРМАЦИЯ:\n");
    printf("  - Год данных: %d\n", data[0].year);  // Предполагаем, что все данные за один год
    printf("  - Всего обработано записей: %d\n", total_records);

    //Подсчет месяцев с данными
    int months_with_data = 0;
    for (int month = 1; month <= 12; month++) {
        if (monthly_count[month] > 0) {
            months_with_data++;
        }
    }
    printf("  - Месяцев с данными: %d из 12\n", months_with_data);
}
