CC = g++
CFLAGS = -Wall -std=c++14
DEPS = client_header.h cl_global.h
P_FLAGS = -lssl -lcrypto -lpthread
OBJ = cl_global.o mtorrent_generator.o client_helper.o client_as_receiver.o client_as_server.o client.o 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

client_2018201034: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(P_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o client_2018201034
