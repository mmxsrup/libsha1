#ifndef __SHA1_H__
#define __SHA1_H__

#include <stdint.h>
#include <sys/types.h>

typedef struct {
	uint32_t hash[5]; // hash value
	size_t count;
	unsigned char buffer[64]; // a single 512 bit block buffer
} SHA1_CTX;


void SHA1_Init(SHA1_CTX* context);

void SHA1_Update(SHA1_CTX* context, const unsigned char* data, size_t len);

void SHA1_Final(unsigned char digest[20], SHA1_CTX* context);


#endif