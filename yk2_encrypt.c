#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define uint128_t unsigned __int128

static const char Key[]  = "STarYZgr3DL11";
static const int  KeyLen = 13;
static const uint64_t Seed = 0x79BAA6BB6398B6F7;

uint64_t mul64(uint64_t *dst, uint64_t src)
{
    uint128_t result128 = (uint128_t)*dst * src;
    *dst = (uint64_t)result128;
    return (result128 - *dst) >> 64;
}

uint32_t mul32(uint32_t *dst, uint32_t src)
{
    uint64_t result64 = (uint64_t)*dst * src;
    *dst = (uint32_t)result64;
    return (result64 - *dst) >> 32;
}

uint32_t calculate(char *data, uint64_t sz)
{
    uint32_t checksum = 0;
    uint32_t add = 0;
    uint64_t result64;
    uint32_t result32;
    uint64_t excess64;
    uint32_t excess32;
    uint32_t sz_result;
    uint32_t read;
    
    if (sz < 0x15B0)
        goto step2;
    
    sz_result = sz;
    
    result64 = Seed;
    excess64 = mul64(&result64, sz);
    
    sz_result -= excess64;
    sz_result = ((sz_result >> 1) + excess64) >> 0xC;
    
    sz -= (int64_t)(sz_result * 0x15B0);
    
    for (int s = 0; s < sz_result; ++s)
    {
    
        read = s * 0x15B0;
        for (int i = 0; i < 0x15B0; ++i)
        {
            add += (uint8_t)data[i + read];
            checksum += add;
        }
        goto step4;
step1:
        continue;
    }
    read = sz_result * 0x15B0;
    
step2:
    if (!sz)
        goto finish;
    
    if (sz < 0x10)
        goto step3;

    result64 = sz >> 4;
    result64 *= 0x10;
    sz -= (int64_t)(result64);
    for (int i = 0; i < result64; ++i)
    {
        add += (uint8_t)data[i + read];
        checksum += add;
    }
    read += result64;


step3:
    if (!sz)
        goto step4;
    
    for (int i = 0; i < sz; ++i)
    {
        add += (uint8_t)data[i + read];
        checksum += add;
    }
    read += sz;
    sz = 0;

step4:
    result32 = 0x80078071;
    excess32 = mul32(&result32, add) >> 0xF;
    add -= (int32_t)(excess32 * 0xFFF1);
    
    result32 = 0x80078071;
    excess32 = mul32(&result32, checksum) >> 0xF;
    checksum -= (int32_t)(excess32 * 0xFFF1);
    if (sz)
        goto step1;
    
finish: 
    checksum = (checksum << 0x10) | add;
    return checksum;
}

int main(int argc, char **argv)
{
    FILE *in, *out;
    unsigned char *data, *outname, *filename, *ext;
    size_t sz;
    uint32_t checksum;
    
    if (argc != 2)
    {
        printf("Usage: %s in.json\n", argv[0]);
        return 0;
    }
    
    if ((in = fopen(argv[1], "rb")) == NULL)
    {
        perror("Error");
        return 1;
    }
    
    // get filename without extension
    filename = malloc(sizeof(argv[1]));
    strcpy(filename, argv[1]);
    ext = strchr(filename, '.');
    filename[(int)(ext - filename)] = '\0';
    
    outname = strcat(filename, ".sav");
    
    if ((out = fopen(outname, "wb")) == NULL)
    {
        perror("Error");
        fclose(in);
        return 1;
    }
    
    printf("Writing %s to %s...\n", argv[1], outname);
    
    fseek(in, 0, SEEK_END);
    sz = ftell(in);
    data = malloc(sz + 4);
    rewind(in);
    fread(data, 1, sz, in);
    fclose(in);
    
    // calculate checksum
    checksum = calculate(data, (uint64_t)sz);
    
    // encrypt
    for (int i = 0; i < sz; i++)
        data[i] ^= Key[i % KeyLen];
    
    // add checksum
    data[sz] = checksum;
    data[sz + 1] = checksum >> 8;
    data[sz + 2] = checksum >> 16;
    data[sz + 3] = checksum >> 24;
    
    fwrite(data, 1, sz + 4, out);
    
    free(data);
    fclose(out);
    
    puts("Done!");
    
    return 0;
}