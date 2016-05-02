TARGET = pro

all: $(TARGET)

$(TARGET):
	g++ -o pro Pro.cpp worker.cpp logger.cpp  -std=c++11 -pthread

clean:
	rm -f *.o *~ $(TARGET)
