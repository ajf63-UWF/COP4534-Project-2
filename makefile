GXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = sim
OBJS = main.o Simulation.o RandomExp.o 

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(GXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp Simulation.h
	$(GXX) $(CXXFLAGS) -c main.cpp

Simulation.o: Simulation.cpp Simulation.h Event.h MinHeap.h FIFOQueue.h RandomExp.h
	$(GXX) $(CXXFLAGS) -c Simulation.cpp

RandomExp.o: RandomExp.cpp RandomExp.h
	$(GXX) $(CXXFLAGS) -c RandomExp.cpp

clean:
	rm -f *.o $(TARGET)