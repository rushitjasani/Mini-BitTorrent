CC = g++
CFLAGS = -Wall -std=c++14 -lssl -lcrypto
DEPS = myheader.h global.h
OBJ = cl_global.o mtorrent_generator.o client.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

term: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf *.o term
