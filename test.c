#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "sha1.h"

#define BUF_SIZE 4096


int main(int argc, char *argv[]) {
	
	if (argc != 2) {
		printf("[Usage] ./test [filename]\n");
		exit(1);
	}
	char* filename = argv[1];

	int fd;
	if ((fd = open(filename, O_RDONLY)) == -1) {
		perror(filename);
		exit(1);
	}

	SHA1_CTX ctx;
	SHA1_Init(&ctx);

	unsigned char buf[BUF_SIZE];
	int size;
	while ((size = read(fd, buf, BUF_SIZE)) > 0) {
		SHA1_Update(&ctx, buf, size);
	}

	unsigned char hash[20];
	SHA1_Final(hash, &ctx);

	for (int i = 0; i < 20; ++i) {
		printf("%02x", hash[i]);
	}
	printf("  %s\n", filename);

	return 0;
}