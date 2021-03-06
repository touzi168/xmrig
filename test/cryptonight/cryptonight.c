#include <unity.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <mm_malloc.h>

#include "options.h"
#include "algo/cryptonight/cryptonight.h"

bool opt_double_hash = false;

const static char input1[152] = {
    0x03, 0x05, 0xA0, 0xDB, 0xD6, 0xBF, 0x05, 0xCF, 0x16, 0xE5, 0x03, 0xF3, 0xA6, 0x6F, 0x78, 0x00,
    0x7C, 0xBF, 0x34, 0x14, 0x43, 0x32, 0xEC, 0xBF, 0xC2, 0x2E, 0xD9, 0x5C, 0x87, 0x00, 0x38, 0x3B,
    0x30, 0x9A, 0xCE, 0x19, 0x23, 0xA0, 0x96, 0x4B, 0x00, 0x00, 0x00, 0x08, 0xBA, 0x93, 0x9A, 0x62,
    0x72, 0x4C, 0x0D, 0x75, 0x81, 0xFC, 0xE5, 0x76, 0x1E, 0x9D, 0x8A, 0x0E, 0x6A, 0x1C, 0x3F, 0x92,
    0x4F, 0xDD, 0x84, 0x93, 0xD1, 0x11, 0x56, 0x49, 0xC0, 0x5E, 0xB6, 0x01,
    0x01, 0x00, 0xFB, 0x8E, 0x8A, 0xC8, 0x05, 0x89, 0x93, 0x23, 0x37, 0x1B, 0xB7, 0x90, 0xDB, 0x19,
    0x21, 0x8A, 0xFD, 0x8D, 0xB8, 0xE3, 0x75, 0x5D, 0x8B, 0x90, 0xF3, 0x9B, 0x3D, 0x55, 0x06, 0xA9,
    0xAB, 0xCE, 0x4F, 0xA9, 0x12, 0x24, 0x45, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x81, 0x46, 0xD4, 0x9F,
    0xA9, 0x3E, 0xE7, 0x24, 0xDE, 0xB5, 0x7D, 0x12, 0xCB, 0xC6, 0xC6, 0xF3, 0xB9, 0x24, 0xD9, 0x46,
    0x12, 0x7C, 0x7A, 0x97, 0x41, 0x8F, 0x93, 0x48, 0x82, 0x8F, 0x0F, 0x02,
};

const static char input2[] = "This is a test";
const static char input3[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus pellentesque metus.";


void cryptonight_av1_aesni(const void* input, size_t size, void* output, struct cryptonight_ctx* ctx);
void cryptonight_av2_aesni_double(const void* input, size_t size, void* output, struct cryptonight_ctx* ctx);
void cryptonight_av3_softaes(const void* input, size_t size, void* output, struct cryptonight_ctx* ctx);
void cryptonight_av4_softaes_double(const void* input, size_t size, void* output, struct cryptonight_ctx* ctx);


static char hash[64];
#define RESULT1        "1a3ffbee909b420d91f7be6e5fb56db71b3110d886011e877ee5786afd080100"
#define RESULT1_DOUBLE "1a3ffbee909b420d91f7be6e5fb56db71b3110d886011e877ee5786afd0801001b606a3f4a07d6489a1bcd07697bd16696b61c8ae982f61a90160f4e52828a7f"
#define RESULT2        "a084f01d1437a09c6985401b60d43554ae105802c5f5d8a9b3253649c0be6605"
#define RESULT3        "0bbe54bd26caa92a1d436eec71cbef02560062fa689fe14d7efcf42566b411cf"


static char *bin2hex(const unsigned char *p, size_t len)
{
    char *s = malloc((len * 2) + 1);
    if (!s) {
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        sprintf(s + (i * 2), "%02x", (unsigned int) p[i]);
    }

    return s;
}


static void * create_ctx(int ratio) {
    struct cryptonight_ctx *ctx = (struct cryptonight_ctx*) _mm_malloc(sizeof(struct cryptonight_ctx), 16);
    ctx->memory = (uint8_t *) _mm_malloc(MEMORY * ratio, 16);

    return ctx;
}


static void free_ctx(struct cryptonight_ctx *ctx) {
    _mm_free(ctx->memory);
    _mm_free(ctx);
}


void test_cryptonight_av1_should_CalcHash(void) {
    struct cryptonight_ctx *ctx = (struct cryptonight_ctx*) create_ctx(1);

    cryptonight_av1_aesni(input1, 76, &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT1, bin2hex(hash, 32));

    cryptonight_av1_aesni(input2, strlen(input2), &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT2, bin2hex(hash, 32));

    cryptonight_av1_aesni(input3, strlen(input3), &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT3, bin2hex(hash, 32));

    free_ctx(ctx);
}


void test_cryptonight_av2_should_CalcHash(void)
{
    struct cryptonight_ctx *ctx = (struct cryptonight_ctx*) create_ctx(2);

    cryptonight_av2_aesni_double(input1, 76, &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT1_DOUBLE, bin2hex(hash, 64));

    free_ctx(ctx);
}


void test_cryptonight_av3_should_CalcHash(void)
{
    struct cryptonight_ctx *ctx = (struct cryptonight_ctx*) create_ctx(1);

    cryptonight_av3_softaes(input1, 76, &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT1, bin2hex(hash, 32));

    cryptonight_av3_softaes(input2, strlen(input2), &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT2, bin2hex(hash, 32));

    cryptonight_av3_softaes(input3, strlen(input3), &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT3, bin2hex(hash, 32));

    free_ctx(ctx);
}


void test_cryptonight_av4_should_CalcHash(void)
{
    struct cryptonight_ctx *ctx = (struct cryptonight_ctx*) create_ctx(2);

    cryptonight_av4_softaes_double(input1, 76, &hash, ctx);
    TEST_ASSERT_EQUAL_STRING(RESULT1_DOUBLE, bin2hex(hash, 64));

    free_ctx(ctx);
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cryptonight_av1_should_CalcHash);
    RUN_TEST(test_cryptonight_av2_should_CalcHash);
    RUN_TEST(test_cryptonight_av3_should_CalcHash);
    RUN_TEST(test_cryptonight_av4_should_CalcHash);

    return UNITY_END();
}
