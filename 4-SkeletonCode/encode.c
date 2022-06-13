#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

OperationType check_operation_type(int argc, char *argv[])
{  
    int st_found = 0;

    for (int i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i],"-st"))
            st_found = 1;
    }  
  

    for (int i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i],"-e"))
            {
                if (!st_found)
                {
                    printf("Invalid Syntax : SecretText file not passed\n");
                    exit(0);
                }
                return e_encode;
            }
        else if (!strcmp(argv[i],"-d"))
            return e_decode;
    } 
    return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc)
{
    int i = 0;
    while (strcmp(argv[i++], "-e"));
    if((i < argc) && strstr(argv[i], ".") && strcmp(strstr(argv[i], "."), ".bmp") == 0 )
    {
        printf("Input image filename : %s\n", argv[i]);        
        encInfo->src_image_fname = argv[i];    
    }
    else
    {
        printf("operation failed\n");
        exit(0);
    }

    i = 0;
    while (strcmp(argv[i++], "-st"));
    if((i < argc) && strstr(argv[i], ".") && strcmp(strstr(argv[i], "."), ".txt") == 0 )
    {
        printf("input Secret filename : %s\n",argv[i]);
        encInfo->secret_fname = argv[i];     
    }    
    else
    {
        printf("operation failed\n");
        exit(0);
    }
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image,EncodeInfo *encInfo)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width of Image : %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height of Image : %u\n", height);

    printf("Capacity of Image : %d Bytes\n", width * height * 3);
    encInfo->image_capacity = width * height * 3;
    // Return image capacity
    return width * height * 3;
}

uint get_textfile_size(FILE *fptr, EncodeInfo *encInfo)
{
    /*Secret text format
        <Magic String #* (16Bytes)>
        <extension size(32Bytes)>
        <extension (depends on extension size)>
        <srcrettext size( 32Bytes )>
        <secret data>        
    */
    fseek(fptr, 0, SEEK_END);
    encInfo->size_secret_file = ftell(fptr) + 16 + 32 + 32 + 32;
    printf("Secret Text Size : %ld Bytes\n", ftell(fptr) + 16 + 32 + 32 +32);
    return ftell(fptr) + 16 + 32 + 32 + 32;
}

Status check_capacity(EncodeInfo *encInfo)
{
    if (encInfo->image_capacity - 54 > encInfo->size_secret_file)
    {
        printf("Image capacilty validated\n");
        printf("Used capacity: %ld / %u\n", encInfo->size_secret_file, encInfo->image_capacity - 54);
        return e_success;
    }
    exit(0);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Copying headers...\n");
    char header[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(header, sizeof(char), 54, fptr_src_image);
    fwrite(header, sizeof(char), 54, fptr_dest_image);
    return e_success;
}

Status close_all_files(EncodeInfo *encInfo)
{
    if (encInfo->fptr_src_image)
        fclose(encInfo->fptr_src_image);
    if (encInfo->fptr_secret)
        fclose(encInfo->fptr_secret);
    if (encInfo->fptr_stego_image)
        fclose(encInfo->fptr_stego_image);    
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	exit(0);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	exit(0);
    }

    // Stego Image file
    encInfo->stego_image_fname = "Stego.bmp";
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	exit(0);
    }

    // No failure return e_success
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8 ; ++i)
    {
        int mask = 1 << (7 - i);
        image_buffer[i] = (image_buffer[i] & 0xFE ) | ((data & mask) >> (7 - i));
    }
}

Status encode_string(const char *string,int len, EncodeInfo *encInfo, const char *success_str)
{
    long int file_position;
    fseek(encInfo->fptr_stego_image,0,SEEK_END);
    file_position = ftell(encInfo->fptr_stego_image);
    fseek(encInfo->fptr_src_image, 0, file_position);
    //fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
    for (int i = 0; i < len; ++i)
    {
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(string[i],encInfo->image_data);
        fwrite(encInfo->image_data,sizeof(char), 8, encInfo->fptr_stego_image);
    }
    printf("%s :: Encoded from %ld\n", success_str,file_position);
    return e_success;
}

Status validate_decode_file(DecodeInfo *decInfo)
{
    unsigned char m_string[2];
    fseek(decInfo->fptr_decode_image, 54, SEEK_SET);
    for (int i = 0; i < 2; ++i)
    {
        fread(decInfo->buffer, sizeof(char), 8, decInfo->fptr_decode_image);
        m_string[i] = decode_byte_from_lsb(decInfo->buffer);
    }
    //fread(m_string, sizeof(char), 2, decInfo->fptr_decode_image);
    if (!strcmp(m_string, "#*"))
        printf("File validation successful\nInitiating decoding sequence");
    else
    {
        printf("Unable to validate file.\nAborting Operation");
        exit(0);
    }
    return (e_success);
}

Status decode_open_files(DecodeInfo *decInfo)
{
    // Decode Image file
    decInfo->fptr_decode_image = fopen(decInfo->decode_fname, "r");
    // Do Error handling
    if (decInfo->fptr_decode_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);

    	exit(0);
    }
    // No failure return e_success
    return e_success;
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int argc)
{
    int i = 0;
    while (strcmp(argv[i++], "-d"));
    if((i < argc) && strstr(argv[i], ".") && strcmp(strstr(argv[i], "."), ".bmp") == 0 )
    {
        printf("Decode image filename : %s\n", argv[i]);        
        decInfo->decode_fname = argv[i];    
    }
    else
    {
        printf("operation failed\n");
        exit(0);
    }

    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char info = 0x00;
    for (int i = 0; i < 8 ; ++i)
    {        
        info = (info << 1) | (image_buffer[i] & 1);
    }
    return info;
}

Status encode_int_to_lsb(unsigned int data, char *image_buffer)
{
    printf("data :%x\n",data);
    for (int i = 0; i < 32 ; ++i)
    {
        unsigned int mask = 1 << (31 - i);
        image_buffer[i] = (image_buffer[i] & 0xFE ) | (data & mask ? 1 : 0);
    }
}

Status encode_integer(const int num, EncodeInfo *encInfo, const char *success_str)
{
    long int file_position;
    fseek(encInfo->fptr_stego_image,0,SEEK_END);
    file_position = ftell(encInfo->fptr_stego_image);
    fseek(encInfo->fptr_src_image, 0, file_position);
    fread(encInfo->image_32B_buffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_int_to_lsb(num,encInfo->image_32B_buffer);
    fwrite(encInfo->image_32B_buffer,sizeof(char), 32, encInfo->fptr_stego_image);
    printf("%s :: Encoded from %ld\n", success_str, file_position);
    return e_success;
}

unsigned int decode_integer_from_lsb(char *image_buffer)
{
    unsigned int info = 0x00000000;
    for (int i = 0; i < 32 ; ++i)
    {    
        info = (info << 1) | (image_buffer[i] & 1);
    }
    return info;
}

Status decode_file(DecodeInfo *decInfo)
{
    unsigned int ext_size, secret_size;
    fseek(decInfo->fptr_decode_image, 54 + 16, SEEK_SET);
    // read the ext_size
    fread(decInfo->buffer_32B,sizeof(char),32,decInfo->fptr_decode_image);
    ext_size = decode_integer_from_lsb(decInfo->buffer_32B);
    printf("ext_size retrieved : %u\n", ext_size);
    // read the extension name 
    fread(decInfo->buffer_32B,sizeof(char),32,decInfo->fptr_decode_image);
    decInfo->extension_name = ".txt";
    // read the length of the secret text
    fread(decInfo->buffer_32B,sizeof(char),32,decInfo->fptr_decode_image);
    secret_size = decode_integer_from_lsb(decInfo->buffer_32B); 
    // read the data
    char st[1000] = {""};
    for (int i = 0; i < secret_size; ++i)
    {
        char info;
        fread(decInfo->buffer,sizeof(char),8,decInfo->fptr_decode_image); 
        info = decode_byte_from_lsb(decInfo->buffer);
        strncat(st,&info,1);
    }
    decInfo->secret_text = st;
    printf("decoded Txt : %s\n", decInfo->secret_text);
    
    return (e_success);
}