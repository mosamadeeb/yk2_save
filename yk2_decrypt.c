#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static const char Key[]  = "STarYZgr3DL11";
static const int  KeyLen = 13;

int main(int argc, char **argv)
{
    FILE *in, *out;
    char *data, *outname, *filename, *ext;
    size_t sz;
    
    if (argc != 2)
    {
        printf("Usage: %s in.sav\n", argv[0]);
        return 0;
    }
    
    if ((in = fopen(argv[1], "rb")) == NULL)
    {
        perror("Error");
        return 1;
    }
    
    filename = malloc(sizeof(argv[1]));
    strcpy(filename, argv[1]);
    ext = strchr(filename, '.');
    filename[(int)(ext - filename)] = '\0';
    
    outname = strcat(filename, ".json");
    
    if ((out = fopen(outname, "wb")) == NULL)
    {
        perror("Error");
        fclose(in);
        return 1;
    }
    
    printf("Writing %s to %s...\n", argv[1], outname);
    
    fseek(in, 0, SEEK_END);
    sz = ftell(in) - 4;
    data = malloc(sz);
    rewind(in);
    fread(data, 1, sz, in);
    fclose(in);
    
    for (int i = 0; i < sz; i++)
        data[i] ^= Key[i % KeyLen];
    
    // remove checksum from end of file to get proper json
    
    fwrite(data, 1, sz, out);
    
    free(data);
    fclose(out);
    
    puts("Done!");
    
    return 0;
}