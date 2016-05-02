CC = g++
CFLAGS = -std=c++11 -Wall -Werror
SRC = Pro.cpp worker.cpp logger.cp
OBJ = $(SRC:.cpp=.o)
LIBS = -lpthread

TARGET = pro

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)
