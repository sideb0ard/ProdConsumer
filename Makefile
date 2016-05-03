TARGET = pro

all: $(TARGET)

$(TARGET):
	g++ -o pro main.cpp worker.cpp logger.cpp  -std=c++11 -pthread

clean:
	rm -f *.o *~ $(TARGET)
