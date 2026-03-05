/**************************************************************************************
D1) ВИД СВЕРХУ
В программе описана структура для хранения двоичного дерева:
typedef struct tree {
datatype key;
struct tree *left, *right;
} tree;

Необходимо реализовать функцию, которая печатает вид дерева сверху. 
Функция должна строго соответствовать прототипу:
void btUpView(tree *root)

# Примеры
# Входные данные: 10 5 15 3 7 13 18 1 6 14 0
# Результат работы: 1 3 5 10 15 18
***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>   // для INT_MIN, чтобы пометить пустые ячейки

// Структура для очереди, которая понадобится при обходе в ширину
typedef struct queue_node {
    tree *node;             // указатель на узел дерева
    int hd;                 // горизонтальное расстояние (horizontal distance) этого узла
    struct queue_node *next; // указатель на следующий элемент очереди
} queue_node;

/*Ф-ЦИЯ ДОБАВЛЕНИЯ ЭЛ-ТА В ОЧЕРЕДЬ
  Принимает: front, rear - указатели на начало и конец очереди (двойные указатели, чтобы изменять их),
  node - указатель на узел дерева,
  hd - горизонтальное расстояние этого узла.*/
void enqueue(queue_node **front, queue_node **rear, tree *node, int hd) {
    queue_node *new_node = (queue_node*) malloc(sizeof(queue_node));   // Создаём новый узел очереди
    new_node->node = node;
    new_node->hd = hd;
    new_node->next = NULL;

    if (*rear == NULL) { // Если очередь пуста, новый элемент становится и началом, и концом
        *front = *rear = new_node;
    } else {             // Иначе добавляем в конец
        (*rear)->next = new_node;
        *rear = new_node;
    }
}

/*Ф-ЦИЯ ИЗВЛЕЧЕНИЯ ЭЛ-ТА ИЗ ОЧЕРЕЖИ
 Принимает: front, rear - указатели на начало и конец очереди.
 Возвращает указатель на извлечённый элемент очереди (нужно будет освободить память).
 Если очередь пуста, возвращает NULL.*/
queue_node* dequeue(queue_node **front, queue_node **rear) {
    if (*front == NULL) return NULL;  // очередь пуста
    queue_node *temp = *front;         // запоминаем текущий первый элемент
    *front = (*front)->next;           // перемещаем начало на следующий элемент
    if (*front == NULL) *rear = NULL;  // если очередь опустела, обнуляем и конец
    return temp;                        // возвращаем указатель на удалённый элемент
}

/*Ф-ЦИЯ С РЕКУРСИЕЙ ДЛЯ ПОИСКА MIN И MAX гориз-го расст-ния (hd) во всём дереве.
  Принимает: root - текущий узел, hd - его горизонтальное расстояние,
  min, max - указатели на переменные, хранящие текущие min и max.*/
void findMinMax(tree *root, int hd, int *min, int *max) {
    if (root == NULL) return;          // если узел пуст, ничего не делаем

    if (hd < *min) *min = hd;// Обновляем минимум и максимум при необходимости
    if (hd > *max) *max = hd;

    findMinMax(root->left, hd - 1, min, max);  // Рекурсивно обходим левое поддерево (hd уменьшается на 1)
    findMinMax(root->right, hd + 1, min, max); // Рекурсивно обходим правое поддерево (hd увеличивается на 1)
}

/*MAIN (печать вида дерева сверху)
 Принимает: root - корень дерева.*/
void btUpView(tree *root) {
    if (root == NULL) return; // Если дерево пустое, ничего не выводим

    // 1. Находим диапазон горизонтальных расстояний (от min до max)
    int min_hd = 0, max_hd = 0;                 // инициализируем текущим корнем (hd=0)
    findMinMax(root, 0, &min_hd, &max_hd);

    // 2. Создаём массив, где индекс будет соответствовать hd, а значение — ключ самого верхнего узла для этого hd.
    int size = max_hd - min_hd + 1;              // количество возможных hd
    int *topView = (int*) malloc(size * sizeof(int));

    // Заполняем массив спец-ым значением INT_MIN, кот-е не мб настоящим ключом (предп-м, что ключи не равны INT_MIN, иначе нужно другое решение)
    for (int i = 0; i < size; i++) {
        topView[i] = INT_MIN;
    }

    /* 3. Выполняем обход в ширину (BFS), чтобы получить узлы по уровням.
    Для каждого узла запоминаем его hd и, если это расстояние встретилось впервые,
    записываем ключ в массив (так как BFS идёт по уровням, первый узел для данного hd* будет самым верхним) */
    queue_node *front = NULL, *rear = NULL;      // инициализируем пустую очередь
    enqueue(&front, &rear, root, 0);              // начинаем с корня, hd=0

    while (front != NULL) {
        queue_node *curr = dequeue(&front, &rear); // Извлекаем элемент из очереди
        tree *node = curr->node;
        int hd = curr->hd;

        int index = hd - min_hd; // Вычисляем индекс в массиве: hd - min_hd
        if (topView[index] == INT_MIN) {
            topView[index] = node->key; // Это первое появление данного hd, сохраняем ключ
        }

        if (node->left != NULL) { // Добавляем в очередь левого и правого детей с обновлёнными hd
            enqueue(&front, &rear, node->left, hd - 1);
        }
        if (node->right != NULL) {
            enqueue(&front, &rear, node->right, hd + 1);
        }

        free(curr);  // освобождаем память, выделенную под элемент очереди
    }

    // 4. Выводим результат: проходим по массиву от min до max и печатаем все ключи, к-рые были записаны (не равны INT_MIN).
    for (int i = 0; i < size; i++) {
        if (topView[i] != INT_MIN) {
            printf("%d ", topView[i]);
        }
    }
    printf("\n");   // переводим строку после вывода

    // Освобождаем память массива
    free(topView);
}