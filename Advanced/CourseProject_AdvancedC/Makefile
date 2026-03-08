# Цели: all (собрать программу), clean (удалить объектные файлы и исполняемый файл)

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g   # -g для отладки, -O2 для оптимизации
LDFLAGS = -lm                             # линковка с математической библиотекой (нужна для fabs и т.д.)

# Исходные файлы
SRCS = main.c functions.c root.c integral.c test.c
# Объектные файлы (заменяем .c на .o)
OBJS = $(SRCS:.c=.o)

# Имя исполняемого файла
TARGET = area

# Правило по умолчанию (all)
all: $(TARGET)

# Линковка объектных файлов в исполняемый
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Компиляция .c в .o (автоматическое правило)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Указание зависимостей (кроме автоматических)
# main.o зависит от всех заголовочных файлов, потому что использует их
# Можно перечислить вручную, но для простоты укажем здесь
main.o: main.c functions.h root.h integral.h test.h
functions.o: functions.c functions.h
root.o: root.c root.h
integral.o: integral.c integral.h
test.o: test.c test.h root.h

# Цель clean: удалить объектные файлы и исполняемый
clean:
	rm -f $(OBJS) $(TARGET)

# Объявляем цели, которые не являются файлами
.PHONY: all clean 
