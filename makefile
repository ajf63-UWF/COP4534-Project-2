GXX = g++
CXXFLAGS = -std=c++17 -02 -Wall -Wextra

OBJS = main.o Simulation.o RandomExp.OBJS

all: Simulation

sim: $(OBJS)
	$(GXX) $(CXXFLAGS) -o sim $(OBJS)

main.o: main.cpp Simulation.h
	$(GXX) $(CXXFLAGS) -c main.cpp

Simulation.o: Simulation.cpp Simulation.h Event.h MinHeap.h FIFOQueue.h RandomExp.h
	$(GXX) $(CXXFLAGS) -c Simulation.cpp

RandomExp.o: RandomExp.cpp RandomExp.h
	$(GXX) $(CXXFLAGS) -c RandomExp.cpp

clean:
	rm -f *.o sim