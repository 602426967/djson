#Makefile for djson
#ver:1.0.1
#update:2013-01-16
CC=g++

CFLAGS := -w #-Wall
LFLAGS := -lm

INCLUDE_PATH := 

FILES := djson.cpp main.cpp 

SRC := $(addprefix ./src/, $(FILES))

FILES := $(SRC:.cpp=.o)

djson: $(FILES)
	g++ -o djson_test $^ $(CFLAGS)
	@echo "build djson finish."
	
.PHONY:clean
clean:
	rm -rf djson_test
	rm -rf $(FILES)
	