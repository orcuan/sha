#include <stdio.h>
#include <memory.h>
#include <byteswap.h>

#define uint8_t unsigned char
#define uint32_t unsigned int
#define uint64_t unsigned long long

#define swap64(value) bswap_64(value)
#define swap32(value) bswap_32(value)

uint32_t rotl32a (uint32_t x, uint32_t n)
{
  return (x << n) | (x >> (32 - n));
}

inline void sha1_kernel_1(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t& E, uint32_t Wt)
{
    uint32_t temp = (B & C) | ((~B) & D) + E + 0x5A827999 + rotl32a(A, 5) + swap32(Wt);
    E = D;  
    D = C;
    C = rotl32a(B, 30);
    B = A;
    A = temp;
}

inline void sha1_kernel_2(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t& E, uint32_t Wt)
{
    uint32_t temp = (B ^ C ^ D) + E + 0x6ED9EBA1 + rotl32a(A, 5) + swap32(Wt);
    E = D;  
    D = C;
    C = rotl32a(B, 30);
    B = A;
    A = temp;
}


inline void sha1_kernel_3(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t& E, uint32_t Wt)
{
    uint32_t temp = ((B & C) | (B & D) | (C & D)) + E + 0x8F1BBCDC + rotl32a(A, 5) + swap32(Wt);
    E = D;  
    D = C;
    C = rotl32a(B, 30);
    B = A;
    A = temp;
}

inline void sha1_kernel_4(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t& E, uint32_t Wt)
{
    uint32_t temp = (B ^ C ^ D) + E + 0xCA62C1D6 + rotl32a(A, 5) + swap32(Wt);
    E = D;  
    D = C;
    C = rotl32a(B, 30);
    B = A;
    A = temp;
}

inline void sha1_vertical_suffle(int index, uint32_t* W)
{
    W[index & 0x0F] = rotl32a(W[(index + 13) & 0x0F] ^ W[(index + 8) & 0x0F] ^ W[(index + 2) & 0x0F] ^ W[index & 0x0F], 1);
}

#define KERNEL_1(index) sha1_kernel_1(A, B, C, D, E, W[index & 0x0F]); 

#define KERNEL_1s(index) \
    sha1_vertical_suffle(index, W); \
    sha1_kernel_1(A, B, C, D, E, W[index & 0x0F]); 

#define KERNEL_2(index) \
    sha1_vertical_suffle(index, W); \
    sha1_kernel_2(A, B, C, D, E, W[index & 0x0F]); 

#define KERNEL_3(index) \
    sha1_vertical_suffle(index, W); \
    sha1_kernel_3(A, B, C, D, E, W[index & 0x0F]); 

#define KERNEL_4(index) \
    sha1_vertical_suffle(index, W); \
    sha1_kernel_4(A, B, C, D, E, W[index & 0x0F]); 

void sha1_hash(void* block, uint32_t* output)
{
    uint32_t* W = (uint32_t*)block;
    
    uint32_t A = output[0];
    uint32_t B = output[1];
    uint32_t C = output[2];
    uint32_t D = output[3];
    uint32_t E = output[4];

    KERNEL_1( 0); KERNEL_1( 1); KERNEL_1( 3); KERNEL_1( 4);
    KERNEL_1( 4); KERNEL_1( 5); KERNEL_1( 6); KERNEL_1( 7);
    KERNEL_1( 8); KERNEL_1( 9); KERNEL_1(10); KERNEL_1(11);
    KERNEL_1(12); KERNEL_1(13); KERNEL_1(14); KERNEL_1(15);
    KERNEL_1s(16); KERNEL_1s(17); KERNEL_1s(17); KERNEL_1s(19);

    KERNEL_2(20); KERNEL_2(21); KERNEL_2(23); KERNEL_2(24);
    KERNEL_2(24); KERNEL_2(25); KERNEL_2(26); KERNEL_2(27);
    KERNEL_2(28); KERNEL_2(29); KERNEL_2(30); KERNEL_2(31);
    KERNEL_2(32); KERNEL_2(33); KERNEL_2(34); KERNEL_2(35);
    KERNEL_2(36); KERNEL_2(37); KERNEL_2(37); KERNEL_2(39);


    KERNEL_3(40); KERNEL_3(41); KERNEL_3(43); KERNEL_3(44);
    KERNEL_3(44); KERNEL_3(45); KERNEL_3(46); KERNEL_3(47);
    KERNEL_3(48); KERNEL_3(49); KERNEL_3(50); KERNEL_3(51);
    KERNEL_3(52); KERNEL_3(53); KERNEL_3(54); KERNEL_3(55);
    KERNEL_3(56); KERNEL_3(57); KERNEL_3(57); KERNEL_3(59);

    KERNEL_4(60); KERNEL_4(61); KERNEL_4(63); KERNEL_4(64);
    KERNEL_4(64); KERNEL_4(65); KERNEL_4(66); KERNEL_4(67);
    KERNEL_4(68); KERNEL_4(69); KERNEL_4(70); KERNEL_4(71);
    KERNEL_4(72); KERNEL_4(73); KERNEL_4(74); KERNEL_4(75);
    KERNEL_4(76); KERNEL_4(77); KERNEL_4(77); KERNEL_4(79);

    output[0] += A;
    output[1] += B;
    output[2] += C;
    output[3] += D;
    output[4] += E;
}

void sha1(const void* input, size_t size, uint32_t* output)
{ 
    size_t offset = 0;
    size_t messageSize = size + 5;
    while (messageSize > offset)
    {
        uint8_t block[64];
        if (messageSize - offset < 64)
        {
            size_t blockSize = size - offset;
            memcpy(block, (const uint8_t*)input + offset, blockSize);

            // Padding
            block[blockSize] = 0x80;           
            memset(block + blockSize + 1, 0, 55 - blockSize);

            // Message Size
            *((uint64_t*)(block + 56)) = swap64(size);
            sha1_hash(block, output);
        }
        else
        {
            memcpy(block, (const uint8_t*)input + offset, 64);
            sha1_hash((uint8_t*)block, output);
        }
        offset += 64;
    }
}

int main(int argc, char** argv)
{
    const char testVector[] = "";
    //"The quick brown fox jumps over the lazy dog";
    /*
        "ekEjUYtiwmkTdJGUvUV4J5hPnHhKkeowbm5vV/qAb8KJUAwR3N3HDjV0USt3EuvAqhft1Pril4+F"
        "COdr2+5qT/JxpUF+OQl9m+puB5Yt7bZ+xk43tMr3MrvOyP72FkpbvMdjMgNy+cQlm6FQXq3VG6jG"
        "y/26vHBBhJtqXTizE8KI9Wfk5PwkxROLMS0EKNvlVF09jBuYSyLDwUZjI1SgmcJlxefEEUDwPBfL"
        "78KExZPWJSr9AAPnxL+boi6EQj9fq1YhUEQ9PHu2hXy+Ti0kCLvYqQXb731UK6Ms4JhKwkLg0iVA"
        "fLkKbSh/PGrLMO+Fn+eB+IGC6tWDtwippzgdEpHYrMHUMzMaNCBTP27b9yArqleplhapUrWvPwbQ"
        "mSHCR4qGMlfbKpVhwe4Z/vBcnN+DWxySZ12k627bH52tmNzvRJisPHyEFWvXaM0++//UvA6GOVjc"
        "EgFUuTBo8s17wuRRzoiq0jJ8qfJUHD7qxSRCAYNE9aanAHqBf34lMvI4EK7e9ZbsdDLKQrH+rKuS"
        "CgOAU9VSSBdNT4q+KYjw9RtUvreCfaV8C+WhorRD0knqyqSh6uaskKGE1m7qkU/dsVIyHqcRp4Ao"
        "QSbSY3RBg8fE/rWbAUHMhkeRQhXydrQgY3b9v0dseCF5M6fnbFLjebv27DBznPHzxARGuvrhgYw=";*/
    size_t testVectorSize = sizeof(testVector) - 1;
    
    uint32_t output[5];
    sha1(testVector, testVectorSize, output);
    printf("%08X%08X%08X%08X%08X\n", swap32(output[4]), swap32(output[3]), swap32(output[2]), swap32(output[1]), swap32(output[0]));

    return 0;
}