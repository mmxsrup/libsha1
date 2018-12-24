CXX = gcc
CFLAGS = -std=c99 -Wall
CFLAGS_SHARED = -fPIC -shared


run: libsha1.so test
	# LD_LIBRARY_PATH=. ./test tmp

libsha1.so: sha1.c sha1.h
	$(CXX) $(CFLAGS) $(CFLAGS_SHARED) -o libsha1.so sha1.c

test: test.c libsha1.so
	$(CXX) -I./ -L./ -o test test.c -lsha1

clean: 
	rm -f *.o libsha1.so test tmp*
