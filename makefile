CXX ?= gcc

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

LDFLAGS = -lstdc++ -lpthread

ITEMPATH ?= ./src 

server: main.cpp threadPool.o
	$(CXX) $(CXXFLAGS) -o server  $^  $(LDFLAGS)

threadPool.o: src/threadPool.h
	$(CXX)  -c src/threadPool.cpp

clean:
	rm  -r server