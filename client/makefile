CC = g++
CFLAGS = -Wall -std=c++14
DEPS = client_header.h cl_global.h
P_FLAGS = -lssl -lcrypto -lpthread
OBJ = cl_global.o mtorrent_generator.o client.o 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(P_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o client