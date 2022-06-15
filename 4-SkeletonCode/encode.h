#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];
    char image_32B_buffer[32];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

typedef struct _DecodeInfo
{
    char *decode_fname;
    FILE *fptr_decode_image;
    char *extension_name;
    char *secret_text;
    char buffer[8];
    char buffer_32B[32];

} DecodeInfo;


/*function prototype */

/* Check operation type */
OperationType check_operation_type(int argc, char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image,EncodeInfo *encInfo );

/* Get file size */
uint get_textfile_size(FILE *fptr, EncodeInfo *encInfo);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_string(const char *string, int len, EncodeInfo *encInfo,const char *success_str);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(EncodeInfo *encInfo);

/* Close all the files */
Status close_all_files(EncodeInfo *encInfo);

/* Validate the decode operation by finding magic string*/
Status validate_decode_file(DecodeInfo *decInfo);

/* openn the files required for decode operation */
Status decode_open_files(DecodeInfo *decInfo);

/* read and validate arguments in decode operation */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int argc);

/* decode a byte from a image buffer of 8Bytes */
char decode_byte_from_lsb(char *image_buffer);

/* Encode Integer to the LSB of Image*/
Status encode_int_to_lsb(unsigned int data, char *image_buffer);

/* write the updated value to the image */
Status encode_integer(const int num, EncodeInfo *encInfo, const char *success_str);

/* Function to decode the image */
Status decode_file(DecodeInfo *decInfo);

/* decode integer value from LSB */ 
unsigned int decode_integer_from_lsb(char *image_buffer);

/* Read the secret text from file */
void read_secret_text(EncodeInfo *encInfo, char text[]);
#endif
