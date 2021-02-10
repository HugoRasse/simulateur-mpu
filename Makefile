CC = gcc
FILES = src/*.c src/*.h
FLAGS= -Wall -Wextra -g

tp1: src/main.c
	${CC} ${FILES} ${FLAGS} -o Simulateur

