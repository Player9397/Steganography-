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
    
    if (!st_found)
    {
        printf("Invalid Syntax : SecretText file not passed\n");
        exit(0);
    }

    for (int i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i],"-e"))
            return e_encode;
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

    encInfo->image_capacity = width * height * 3;
    // Return image capacity
    return width * height * 3;
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
