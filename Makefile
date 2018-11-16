
CC = g++
CFLAGS = -std=c++11 -lpthread -lm
NCURSES_FLAGS = -lncurses

PORTAUDIO_FLAGS = -lportaudio
DIR = sources
DIR1 = servidor
DIR2 = cliente
TARGET = snakeserver
ClIENTE_TARGET = snakeclient
SRCS := $(shell find $(DIR) -name '*.cpp')
OBJS = $(SRCS:.c=.o)
SRCS2 := $(shell find $(DIR1) -name '*.cpp')
OBJS2 = $(SRCS2:.c=.o)
SRCS3 := $(shell find $(DIR2) -name '*.cpp')
OBJS3 = $(SRCS3:.c=.o)
.PHONY: depend clean

all:$(TARGET) $(ClIENTE_TARGET)

$(TARGET):$(OBJS) $(OBJS2)
	$(CC) -o $@ $^ $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

$(ClIENTE_TARGET):$(OBJS) $(OBJS3)
	$(CC) -o $@ $^ $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

	
%.o:%.cpp
	$(CC) $(SRCS) $(SRCS3) $(CFLAGS) -c $< -o $@

run:all 
	./$(TARGET)

clean:
	$(RM) ./$(TARGET) ./$(ClIENTE_TARGET)
	$(RM) $(DIR)/*.o