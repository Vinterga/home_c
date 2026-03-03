#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define MIN_Y  2

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)}; //направления
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, SEED_NUMBER=3}; //размеры

//!!!! ВНЕСЕНА ПРАВКА!!!!!
#define CONTROLS 3 // Кол-во различных наборов управления (стрелки, WSAD нижний регистр, WSAD верхний регистр)

// Здесь храним коды управления змейкой (Структура для хранения кодов клавиш, соответствующих направлениям)
struct control_buttons
{
    int down;  // клавиша для движения вниз
    int up;    // клавиша для движения вверх
    int left;  // клавиша для движения влево
    int right; // клавиша для движения вправо
} control_buttons;

//!!!! ВНЕСЕНА ПРАВКА!!!!! добавлены WASD в обоих регистрах
struct control_buttons default_controls[CONTROLS] = {
    {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},   // стрелки
    {'s', 'w', 'a', 'd'},                      // нижний регистр WSAD
    {'S', 'W', 'A', 'D'}                       // верхний регистр WSAD
};
/*Добавлен массив default_controls[CONTROLS]
Вместо одной структуры теперь массив из трёх структур. 
Каждая структура — это один набор клавиш для направлений.
Добавлен набор для нижнего регистра ('s','w','a','d') и для верхнего ('S','W','A','D').*/

/* Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост*/
 //!!!! ВНЕСЕНА ПРАВКА!!!!! структура змейки больше не содержит поле controls
typedef struct snake_t
{
    int x;                // координата головы по X
    int y;                // координата головы по Y
    int direction;        // текущее направление
    size_t tsize;         // текущий размер хвоста
    struct tail_t *tail;  // указатель на массив хвоста
} snake_t;

/*Хвост - это массив, состоящий из координат x,y*/
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

/* Еда — это массив точек, состоящий из координат x,y, времени,
 когда данная точка была установлена, и поля, сигнализирующего,
 была ли данная точка съедена.*/
struct food
{
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

// Ф-ции для работы с едой
void initFood(struct food f[], size_t size)
{
    struct food init = {0,0,0,0,0};
    for(size_t i = 0; i < size; i++)
    {
        f[i] = init;
    }
}

/*Обновить/разместить текущее зерно на поле*/
void putFoodSeed(struct food *fp)
{
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    mvprintw(fp->y, fp->x, "%s", spoint);
}

/*Разместить еду на поле*/
void putFood(struct food f[], size_t number_seeds)
{
    for(size_t i=0; i<number_seeds; i++)
    {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood)
{
    for(size_t i=0; i<nfood; i++)
    {
        if( f[i].put_time )
        {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS )
            {
                putFoodSeed(&f[i]);
            }
        }
    }
}

// Ф-ции для работы со змейкой
void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t={0,0};
    for(size_t i=0; i<size; i++)
    {
        t[i]=init_t;
    }
}

void initHead(struct snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

//!!!!!! ВНЕСЕНЫ ПРАВКИ (удалено поле)
void initSnake(snake_t *head, size_t size, int x, int y)
{
tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail;     // прикрепляем к голове хвост
    head->tsize = size+1; // начальный размер хвоста + голова (в оригинале так)
    //head->controls = default_controls;
    // Поля head->controls больше нет, поэтому удаляем присваивание
}

//!!!!!! ВНЕСЕНЫ ПРАВКИ (обновление координат)
// Движение головы 
void go(struct snake_t *head)
{
    char ch = '@';
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(head->y, head->x, " "); // стираем старую позицию головы
    switch (head->direction)
    {
        case LEFT:
            if(head->x <= 0)
                head->x = max_x;   // если ушли влево за границу, перекидываем вправо
            mvprintw(head->y, --(head->x), "%c", ch);
            break;
        case RIGHT:
            mvprintw(head->y, ++(head->x), "%c", ch);
            break;
        case UP:
            mvprintw(--(head->y), head->x, "%c", ch);
            break;
        case DOWN:
            mvprintw(++(head->y), head->x, "%c", ch);
            break;
        default:
            break;
    }
    refresh();
}

//!!! ДОБАВЛЕНО
// Движение хвоста
void goTail(struct snake_t *head)
{
    char ch = '*';
    // стираем последний элемент хвоста
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    // сдвигаем хвост: каждый элемент становится на место предыдущего
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    // первый элемент хвоста становится на место, где только что была голова
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

/* Функция для определения направления по коду нажатой клавиши.
  Проходим по всем наборам управления (default_controls) и проверяем,
  совпадает ли клавиша с одной из заданных для какого-либо направления.
  key - код нажатой клавиши
  Возвращает направление (LEFT, UP, RIGHT, DOWN) или 0, если клавиша не управляющая*/
  int getDirectionByKey(int32_t key)
{
    for (int i = 0; i < CONTROLS; i++)
    {
        if (key == default_controls[i].down)  return DOWN;
        if (key == default_controls[i].up)    return UP;
        if (key == default_controls[i].left)  return LEFT;
        if (key == default_controls[i].right) return RIGHT;
    }
    return 0; // клавиша не найдена в управлении
}

/* Ф-ция checkDirection роверяет, можно ли сменить направление с текущего на новое,
соответствующее нажатой клавише. Запрещает разворот на 180 градусов.
 snake - указатель на змейку
 key - код нажатой клавиши
 возвращает 1, если направление можно изменить, иначе 0*/
int checkDirection(snake_t* snake, int32_t key)
{
    int new_dir = getDirectionByKey(key);
    if (new_dir == 0) return 0; // клавиша не управляет змейкой

    // Проверка на противоположные направления
    if ((snake->direction == UP && new_dir == DOWN) ||
        (snake->direction == DOWN && new_dir == UP) ||
        (snake->direction == LEFT && new_dir == RIGHT) ||
        (snake->direction == RIGHT && new_dir == LEFT))
    {
        return 0; // запрещённый разворот
    }
    return 1; // направление допустимо
}

/* Ф-ция changeDirection Изменяет направление змейки, если клавиша допустима и не запрещена.
Теперь использует checkDirection для проверки.*/
void changeDirection(snake_t* snake, const int32_t key)
{
    if (checkDirection(snake, key))
    {
        // Если проверка пройдена, определяем новое направление и устанавливаем его
        int new_dir = getDirectionByKey(key);
        if (new_dir != 0) // на всякий случай (хотя checkDirection уже гарантирует)
        {
            snake->direction = new_dir;
        }
    }
    // Если проверка не пройдена, просто ничего не делаем (змейка продолжает движение в прежнем направлении)
}

int main()
{
snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake,START_TAIL_SIZE, 10, 10);

    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();             // Отключаем echo() режим при вызове getch
    curs_set(FALSE);      // Отключаем курсор
    mvprintw(0, 0,"Use arrows for control. Press 'F10' for EXIT");
    timeout(0);           // Отключаем таймаут после нажатия клавиши в цикле
    
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER); // Кладем зерна
    
    int key_pressed=0;
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch();           // Считываем клавишу
        go(snake);
        goTail(snake);
        timeout(100);                   // Задержка при отрисовке
        refreshFood(food, SEED_NUMBER); // Обновляем еду
        changeDirection(snake, key_pressed);
    }
    
    free(snake->tail);
    free(snake);
    endwin(); // Завершаем режим curses mod
    return 0;
}
