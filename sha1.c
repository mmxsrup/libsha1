#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "sha1.h"

#define BLOCK_BYTE_SIZE 64

// left rotate
#define lrol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

void debug_print(SHA1_CTX* context) {
	printf("-----------------------\n");
	printf("count: %ld\n", context->count);

	for (int i = 0; i < 5; ++i)
		printf("%x ", context->hash[i]);
	printf("\n");

	for (int i = 0; i < 64; ++i) {
		printf("%x ", context->buffer[i]);
		if (i % 8 == 7) printf("\n");
	}
	printf("\n");
}


/* Hash a single 512 bit block (private function) */
void SHA1_Transform(SHA1_CTX* context) {
	uint32_t w[80];
	for (int i = 0; i < 16; ++i) {
		w[i]  = context->buffer[(i << 2)] << 24;
		w[i] |= context->buffer[(i << 2) + 1] << 16;
		w[i] |= context->buffer[(i << 2) + 2] << 8;
		w[i] |= context->buffer[(i << 2) + 3];
	}

	for (int i = 16; i < 80; ++i) {
		w[i] = lrol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
	}

	// Initialize hash value for this chunk
	uint32_t a = context->hash[0];
	uint32_t b = context->hash[1];
	uint32_t c = context->hash[2];
	uint32_t d = context->hash[3];
	uint32_t e = context->hash[4];

	// Main loop
	for (int i = 0; i < 80; ++i) {
		uint32_t f;
		uint32_t k;
		if (0 <= i && i <= 19) {
			f = (b & c) | ((~b) & d);
			k = 0x5A827999;
		} else if (20 <= i && i <= 39) {
			f = b ^ c ^ d;
			k = 0x6ED9EBA1;
		} else if (40 <= i && i <= 59) {
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		} else if (60 <= i && i <= 79) {
			f = b ^ c ^ d;
			k = 0xCA62C1D6;
		} else {
			assert(0);
		}

		uint32_t tmp = lrol(a, 5) + f + e + k + w[i];
		e = d;
		d = c;
		c = lrol(b, 30);
		b = a;
		a = tmp;
	}

	// Add this chunk's hash to result
	context->hash[0] += a;
	context->hash[1] += b;
	context->hash[2] += c;
	context->hash[3] += d;
	context->hash[4] += e;
}


void SHA1_Init(SHA1_CTX* context) {
	context->hash[0] = 0x67452301;
	context->hash[1] = 0xEFCDAB89;
	context->hash[2] = 0x98BADCFE;
	context->hash[3] = 0x10325476;
	context->hash[4] = 0xC3D2E1F0;
	context->count = 0;
}


void SHA1_Update(SHA1_CTX* context, const unsigned char* data, size_t len) {
	// prev used context->buffer's size
	size_t used_size = context->count % BLOCK_BYTE_SIZE;

	// Calc hash one block at a time
	size_t idx = 0; // next data's index
	// printf("used_size %ld\n", used_size);
	// debug_print(context);

	while (used_size + len > 63) {
		memcpy(&context->buffer[used_size], &data[idx], 64 - used_size);
		SHA1_Transform(context);
		context->count += 64 - used_size;
		used_size = 0;
		idx += 64 - used_size;
		len -= 64 - used_size;
	}

	// Save remaining part in buffer
	// printf("len %ld\n", len);
	memcpy(&context->buffer[used_size], &data[idx], len);
	context->count += len;
	idx += len;
	len -= len;

	// debug_print(context);

	assert(len == 0);
	// printf("context->count %ld idx %ld\n", context->count, idx);
	assert(context->count == used_size + idx);
}


void SHA1_Final(unsigned char digest[20], SHA1_CTX* context) {
	size_t data_size = context->count;
	// debug_print(context);

	// Padding processf
	context->buffer[context->count] = 0x80;
	context->count += 1;

	size_t used_size = context->count % BLOCK_BYTE_SIZE;

	if (used_size > 56) { // Is not enough buffer
		for (int i = used_size; i < BLOCK_BYTE_SIZE; ++i) {
			context->buffer[i] = 0x0;
		}
		context->count += BLOCK_BYTE_SIZE - used_size;
		SHA1_Transform(context);
	}
	
	used_size = context->count % BLOCK_BYTE_SIZE;
	for (int i = used_size; i < 56; ++i) {
		context->buffer[i] = 0x0;
	}
	for (int i = 0; i < 8; ++i) {
		context->buffer[56 + i] = ((data_size * 8) >> (8 * (8 - i - 1))) & 0xff;
	}

	SHA1_Transform(context);

	// debug_print(context);

	// Write the result
	for (int i = 0; i < 5; ++i) {
		// printf("context %x\n", context->hash[i]);
		for (int j = 0; j < 4; ++j) {
			digest[(i << 2) + j] = (unsigned char)((context->hash[i] >> (8 * (3 - j))) & 0xff);
			// printf("%x ", digest[(i << 2) + j]);
		}
		// printf("\n");
	}

}