#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define MIN_Y  2
double DELAY = 0.1;
#define PLAYERS  2
#define MODE_SINGLE 1
#define MODE_MULTI  2
#define MODE_BATTLE 3



enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10,SEED_NUMBER=3,CONTROLS=2};


// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
} control_buttons;

struct control_buttons default_controls[CONTROLS] = {{KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},
                                                    {'s', 'w', 'a', 'd'}};

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
typedef struct snake_t
{
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t *tail;
    struct control_buttons controls;
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;
/*
 Еда — это массив точек, состоящий из координат x,y, времени,
 когда данная точка была установлена, и поля, сигнализирующего,
 была ли данная точка съедена.
 */
struct food
{
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

void initFood(struct food f[], size_t size)
{
    struct food init = {0,0,0,0,0};
    for(size_t i=0; i<size; i++)
    {
        f[i] = init;
    }
}

int startMenu() {
    int choice = 0;
    clear(); // очищаем экран
    mvprintw(5, 10, "=== SNAKE GAME ===");
    mvprintw(7, 10, "1. Одиночная игра");
    mvprintw(8, 10, "2. Два игрока");
    mvprintw(9, 10, "3. Битва с ботом");
    mvprintw(11, 10, "Выберите режим (1-3): ");
    refresh();

    choice = getch() - '0'; // преобразуем символ в число
    return choice;
}

/*Обновить/разместить текущее зерно на поле */
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

/*
 Разместить еду на поле
 */
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
void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t= {0,0};
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
//========================================================================
void initSnake(snake_t *head[], size_t size, int x, int y,int i)
{
    head[i]    = (snake_t*)malloc(sizeof(snake_t));
tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head[i], x, y);
    head[i]->tail     = tail; // прикрепляем к голове хвост
    head[i]->tsize    = size+1;
    //~ head[i]->controls = default_controls[i];
    head[i]->controls = default_controls[0];
}

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake_t *head)
{
    char ch = '@';
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction)
    {
    case LEFT:
        if(head->x <= 0) // Циклическое движение, чтобы не
            // уходить за пределы экрана
            head->x = max_x;
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

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake_t *head)
{
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

//========================================================================
//Проверка того, является ли какое-то из зерен съеденным,
_Bool haveEat(struct snake_t *head, struct food f[]) // head – указатель на змейку, f[] – массив еды
{
    for (size_t i = 0; i < MAX_FOOD_SIZE; i++) {                     //Проходим по всем возможным кусочкам еды (до MAX_FOOD_SIZE)
        if (f[i].enable && head->x == f[i].x && head->y == f[i].y) { // Если зерно активно (enable == 1) и его координаты совпадают с головой
            f[i].enable = 0;      // зерно съедено – отключаем его
            return 1;             // возвращаем истину (еда найдена)
        }
    }
    return 0; // ничего не съели
/* size_t i – счётчик цикла (тип size_t – беззнаковый целый, подходит для индексов).
f[i].enable – если зерно активно (1), то проверяем координаты
head->x – координата головы по X, head->y – по Y.
Если совпали – отключаем зерно и возвращаем 1 (истина).
Если цикл закончился без совпадений – возвращаем 0.*/
}

/*Увеличение хвоста на 1 элемент (нужно просто увеличить поле tsize, но не забыть проверить, что не превышен максимальный размер)*/
void addTail(struct snake_t *head)
{
    if (head->tsize < MAX_TAIL_SIZE) { // Проверяем, не достигнут ли максимум
        head->tsize++;                 // увеличиваем длину хвоста
    } // Если достигнут максимум – ничего не делаем (хвост не растёт)
}
/*head->tsize – текущая длина хвоста (сколько сегментов). При съедении еды она увеличивается.
Проверка head->tsize < MAX_TAIL_SIZE нужна, чтобы не выйти за границы массива tail (выделено MAX_TAIL_SIZE элементов).
Просто увеличиваем – при следующем движении новый сегмент автоматически появится благодаря логике goTail (там массив сдвигается, и последний сегмент перезаписывается).*/

//========================================================================
int checkDirection(snake_t* snake, int32_t key) //Проверяет, можно ли сменить направление на то, которое соответствует нажатой клавише. Запрещает разворот на 180 градусов. Если клавиша не управляющая – возвращает 0.
{
    // Определяем, какое направление соответствует этой клавише
    // Сравниваем с каждым полем структуры controls
    int new_dir = 0; // 0 – неизвестное направление

    if (key == snake->controls.down) new_dir = DOWN;
    else if (key == snake->controls.up) new_dir = UP;
    else if (key == snake->controls.left) new_dir = LEFT;
    else if (key == snake->controls.right) new_dir = RIGHT;
    
    if (new_dir == 0) return 0; // Если клавиша не соответствует ни одному направлению – нельзя менять

    // Проверяем, не пытается ли змейка развернуться на 180 градусов
    // Текущее направление хранится в snake->direction
    if ((snake->direction == UP && new_dir == DOWN) ||
        (snake->direction == DOWN && new_dir == UP) ||
        (snake->direction == LEFT && new_dir == RIGHT) ||
        (snake->direction == RIGHT && new_dir == LEFT))
    {
        return 0; // запрещённый (прием) разворот
    }

    return 1;     //Все проверки пройдены – направление можно менять

}
/*Сначала мы определяем, какое направление соответствует нажатой клавише. Для этого сравниваем key со значениями из структуры controls (они заданы для каждой змейки отдельно).
Если совпадения нет – возвращаем 0 (нельзя менять).
Затем проверяем противоположные пары: если змейка ползёт вверх, а нажата клавиша вниз – это разворот, запрещаем. Аналогично для других направлений.
Если всё хорошо – возвращаем 1.*/


//Вынести тело цикла while из int main() в отдельную функцию update и посмотреть, как изменится профилирование
void update(struct snake_t *head, struct food f[], const int32_t key)
{
    clock_t begin = clock();
    go(head);
    goTail(head);
    if (checkDirection(head,key))
    {
        changeDirection(head, key);
    }
    refreshFood(food, SEED_NUMBER);// Обновляем еду
    if (haveEat(head,food))
    {
        addTail(head);
    }
    refresh();//Обновление экрана, вывели кадр анимации
    while ((double)(clock() - begin)/CLOCKS_PER_SEC<DELAY)
    {}
}
//========================================================================

_Bool isCrush(snake_t * snake) //Проверяет, не столкнулась ли голова с каким-либо сегментом хвоста (кроме головы, кнчн). Если да – возвращает 1.
{
    for (size_t i = 0; i < snake->tsize; i++) { // Перебираем все сегменты хвоста (индекс 0 – первый после головы)
        if (snake->tail[i].x == snake->x && snake->tail[i].y == snake->y) { // Если координаты сегмента совпадают с головой
            return 1; // столкновение!
        }
    }
    return 0; // столкновения нет
}
/*Цикл по всем сегментам хвоста от 0 до tsize-1.
Для каждого проверяем, равны ли его координаты координатам головы.
Если да – сразу возвращаем 1 (истина).
Если цикл завершился без совпадений – возвращаем 0.*/


//========================================================================
// Корректировка положения еды
// f[] – массив еды
// nfood – количество активных зерен (SEED_NUMBER)
// head – указатель на змейку
void repairSeed(struct food f[], size_t nfood, struct snake_t *head) //Исправляет некорректное расположение еды: убирает еду, попавшую на хвост змейки + убирает ситуации, когда два зерна находятся в одной клетке
{
     // 1. Проверяем, не попала ли еда на хвост
    for (size_t i = 0; i < head->tsize; i++) {           // для каждого сегмента хвоста
        for (size_t j = 0; j < nfood; j++) {             // для каждого зерна
            if (f[j].enable && f[j].x == head->tail[i].x && f[j].y == head->tail[i].y) { // Если зерно активно и его координаты совпадают с сегментом хвоста
                putFoodSeed(&f[j]); // переставить зерно на новое случайное место
                // После перестановки проверка продолжится с этим же j, но, поскольку зерно переместилось, оно уже может не совпадать.
            }
        }
    }

    // 2. Проверяем, нет ли двух зерен в одной клетке
    for (size_t i = 0; i < nfood; i++) {                 // для каждого зерна
        for (size_t j = i + 1; j < nfood; j++) {         // для всех следующих за i
            if (f[i].enable && f[j].enable && f[i].x == f[j].x && f[i].y == f[j].y) { //Если оба активны и координаты совпадают, то
                putFoodSeed(&f[j]); // переставить второе зерно (можно переставить и первое, но так проще)
            }
        }
    }
} /*- В первом двойном цикле проходим по всем сегментам хвоста и всем зернам. Если зерно активно и его координаты совпадают 
с сегментом хвоста – вызываем putFoodSeed, которое перемещает зерно в случайную свободную позицию 
(не проверяя, правда, не попадёт ли оно опять на хвост, но это будет исправлено при следующем вызове repairSeed).
- Во втором цикле сравниваем каждую пару зерен. Начинаем j с i+1, чтобы не сравнивать зерно само с собой. 
Если оба активны и координаты равны – переставляем второе (можно и первое).
- Функция putFoodSeed уже определена ранее – она затирает старое положение пробелом, генерирует новые координаты, обновляет время и включает зерно.*/


int main()
{
    int mode = startMenu(); // В зависимости от режима можно менять количество игроков (PLAYERS) и другие параметры. Например, если mode == 1, то PLAYERS = 1; если mode == 2, то PLAYERS = 2 и т.д.
//========================================================================   
snake_t* snakes[PLAYERS];
    for (int i = 0; i < PLAYERS; i++)
        initSnake(snakes,START_TAIL_SIZE,10+i*10,10+i*10,i);
//========================================================================

    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();             // Отключаем echo() режим при вызове getch
    curs_set(FALSE);      //Отключаем курсор
    mvprintw(0, 0,"Использовать клавиши стрелок для управления. Нажмите 'F10' для выхода.");
    timeout(0);           //Отключаем таймаут после нажатия клавиши в цикле
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER);// Кладем зерна
    int key_pressed=0;
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch(); // Считываем клавишу
        for (int i = 0; i < PLAYERS; i++)
        {
            update(snakes[i], food, key_pressed);
            if(isCrush(snakes[i]))
                break;//!!!!!!
            repairSeed(food, SEED_NUMBER, snakes[i]);
        }
    }
    for (int i = 0; i < PLAYERS; i++)
    {
        //printExit(snakes[i]);
        free(snakes[i]->tail);
        free(snakes[i]);
    }
    endwin(); // Завершаем режим curses mod
    return 0;
}
