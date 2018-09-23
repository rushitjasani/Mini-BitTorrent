CC = g++
CFLAGS = -Wall -std=c++14
DEPS = server_header.h server_global.h
P_FLAGS = -lssl -lcrypto -lpthread
OBJ = server_global.o tracker_helper.o tracker.o 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

tracker_2018201034: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(P_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o tracker_2018201034
