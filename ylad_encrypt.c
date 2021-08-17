#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static const char Key[]  = "STarYZgr3DL11";
static const int  KeyLen = 13;

// https://stackoverflow.com/a/21001712
unsigned int crc32b(unsigned char *message) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

uint32_t calculate(char *data, uint64_t sz)
{
    return crc32b(data);
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