#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define MIN_Y  2
enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10};


// Здесь храним коды управления змейкой (по умолчанию – стрелки)
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
} control_buttons;

struct control_buttons default_controls = {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT};

/* Голова змейки содержит в себе x,y - координаты текущей позиции
   direction - направление движения
   tsize - размер хвоста
   *tail -  ссылка на хвост */
typedef struct snake_t
{
    int x;               // координата головы
    int y;
    int direction;       // направление
    size_t tsize;        // текущая длина хвоста
    struct tail_t *tail; // массив хвоста
    struct control_buttons controls;
} snake_t;

// Хвост - это массив, состоящий из координат x,y
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

//Инициализация хвоста (заполнение нулями)
void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t = {0,0};
    for(size_t i = 0; i < size; i++)
    {
        t[i]  = init_t;
    }
}

//Инициализация головы 
void initHead(struct snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

//Инициализация всей смейки
void initSnake(snake_t *head, size_t size, int x, int y)
{
tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE * sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail;       // прикрепляем к голове хвост
    head->tsize = size + 1;  // +1, потому что голова отдельно? В оригинале так, оставлю
    head->controls = default_controls;
}

//!!!!! МЕНЯЛА ЗДЕСЬ!!!!!!!
// Добавлена обработка всех границ экрана – при выходе за любую границу змейка появляется 
// с противоположной стороны (циклическое движение).
// Движение головы с учетом текущего направления движения (только изменение координат, без отрисовки)
void go(struct snake_t *head)
{
     int new_x = head->x;
    int new_y = head->y;

    switch (head->direction)
    {
        case LEFT:  new_x--; break;
        case RIGHT: new_x++; break;
        case UP:    new_y--; break;
        case DOWN:  new_y++; break;
    }

    // Коррекция границ (циклическое движение)
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    if (new_x < 0) new_x = max_x - 1;
    if (new_x >= max_x) new_x = 0;
    if (new_y < MIN_Y) new_y = max_y - 1;
    if (new_y >= max_y) new_y = MIN_Y;

    head->x = new_x;
    head->y = new_y;
}

//Изменение направления при нажатии клавиш
void changeDirection(struct snake_t* snake, const int32_t key)
{
    if (key == snake->controls.down)
        snake->direction = DOWN;
    else if (key == snake->controls.up)
        snake->direction = UP;
    else if (key == snake->controls.right)
        snake->direction = RIGHT;
    else if (key == snake->controls.left)
        snake->direction = LEFT;
}

//!!!!! МЕНЯЛА ЗДЕСЬ!!!!!!!
// Исправлена логика хвоста – теперь хвост движется правильно, 
// Голова и первый сегмент хвоста не перекрываются.
//Движение хвоста с учетом движения головы. Принимает старые координаты головы (до движения)
void goTail(struct snake_t *head, int old_x, int old_y)
{
    // Стираем последний элемент хвоста
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");

    // Сдвигаем хвост (от конца к началу)
    for (size_t i = head->tsize-1; i > 0; i--)
    {
        head->tail[i] = head->tail[i-1];
    }

    // Вставляем старую позицию головы в начало хвоста
    head->tail[0].x = old_x;
    head->tail[0].y = old_y;

    // Рисуем весь хвост (голова будет нарисована отдельно)
    for (size_t i = 0; i < head->tsize; i++)
    {
        mvprintw(head->tail[i].y, head->tail[i].x, "*");
    }
}

//!!!!! НОВАЯ ФУНКЦИЯ!!!!!!!
// Проверка столкновения головы с хвостом: если голова наезжает на любой сегмент хвоста, 
// игра завершается с сообщением.
int isCrash(snake_t *snake)
{
    for (size_t i = 0; i < snake->tsize; i++)
    {
        if (snake->tail[i].x == snake->x && snake->tail[i].y == snake->y)
            return 1;
    }
    return 0;
}

//!!!!! МЕНЯЛА ЗДЕСЬ!!!!!!!
int main()
{
    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake, START_TAIL_SIZE, 10, 10);
    
    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();             // Отключаем echo() режим при вызове getch
    curs_set(FALSE);      // Отключаем курсор
    mvprintw(0, 0,"Use arrows for control. Press 'F10' for EXIT");
    timeout(0);           // getch() не блокирует (возвращает ERR, если нет клавиши)
    
    int key_pressed  = 0;
    int game_over = 0;    // Завершение работы программы, когда станет 1
    
    //!!!!! ЦИКЛ ПЕРЕПИСАН!!!!!!!
    while( key_pressed != STOP_GAME && !game_over)
    {
        clock_t start = clock();   // засекаем начало кадра
        /*Заменён timeout(100) на собственную задержку через clock() – 
        теперь во время ожидания программа продолжает опрашивать клавиши, 
        что делает управление более отзывчивым.*/

        // Сохраняем старые координаты головы
        int old_x = snake->x;
        int old_y = snake->y;

        go(snake);                  // вычисляем новые координаты головы
        goTail(snake, old_x, old_y); // движение и отрисовка хвоста

        // Отрисовка головы (поверх хвоста)
        mvprintw(snake->y, snake->x, "@");
        refresh();

        // Проверка столкновения с хвостом
        if (isCrash(snake))
        {
            game_over = 1;
            break;
        }

        // Ожидание до конца кадра (100 мс) с опросом клавиш
        while (((clock() - start) * 1000 / CLOCKS_PER_SEC) < 100)
        {
            int ch = getch();
            if (ch != ERR)
            {
                changeDirection(snake, ch);
                if (ch == STOP_GAME) key_pressed = ch;
            }
        }
}

 // Сообщение о конце игры
    if (game_over)
    {
        mvprintw(MAX_TAIL_SIZE, 0, "GAME OVER! Press any key to exit...");
        refresh();
        getch();   // ждём клавишу перед выходом
    }

    free(snake->tail);
    free(snake);
    endwin();      // завершаем curses режим
    return 0;
}
