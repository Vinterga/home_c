/**************************************************************************************
D2) НАЙТИ БРАТА
В программе описано двоичное дерево:
typedef struct tree {
datatype key;
struct tree *left, *right;
struct tree *parent; //ссылка на родителя
} tree;
Требуется реализовать функцию, которая по ключу возвращает адрес соседнего элемента - брата. 
Если такого ключа нет или у узла нет брата, то необходимо вернуть 0.

Прототип функции: 
tree * findBrother(tree *root, int key)

# Примеры:
# Входные данные: 10 5 15 3 7 13 18 1 6 14 0 3
# Результат работы: 7
***************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

// Структура tree и прототип brother уже даны 

/*ф-ция ищет узел с ключом key и возвращает указатель на его брата.
Если узел не найден или брата нет, возвращает NULL.*/
tree* findBrother(tree *root, int key) {
    tree *current = root;     //Ищем узел с данным ключом (итеративный обход, как в бинарном дереве поиска)
    while (current != NULL) {
        if (key == current->key)
            break;                  // нашли
        else if (key < current->key)
            current = current->left; // идём влево
        else
            current = current->right; // идём вправо
    }

    if (current == NULL) // Если узел не найден,
        return NULL;     //то возвращаем NULL

  
    tree *parent = current->parent; // Получаем родителя
    if (parent == NULL)
        return NULL;                // корень, брата нет

    
    if (parent->left == current) //Определяем, каким ребёнком является наш узел, и возвращаем другого
        return parent->right;    // брат справа (м.б. NULL)
    else
        return parent->left;     // брат слева
}