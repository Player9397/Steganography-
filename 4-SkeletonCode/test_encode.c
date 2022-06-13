#include <stdio.h>
#include "encode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //atexit(func_ptr);
    if (check_operation_type(argc, argv) ==  e_encode)
    {
        read_and_validate_encode_args(argv,&encInfo,argc);
        open_files(&encInfo);
        get_image_size_for_bmp(encInfo.fptr_src_image, &encInfo);
        get_textfile_size(encInfo.fptr_secret,&encInfo);
        check_capacity(&encInfo);
        printf("validation done.\nInitiating encoding operation...\n");
        copy_bmp_header(encInfo.fptr_src_image,encInfo.fptr_stego_image);
        encode_string("#*",2,&encInfo, "Magic String encoded Successfully");
        encode_integer(4, &encInfo, "File Extension Size Encoded Successfully");
        encode_string(".txt",4, &encInfo, "File Extension name Encoded Successfully");
        encode_integer(2,&encInfo, "Secret text length encoded successfully");
        encode_string("Hi",2,&encInfo,"Secret text encoded Successfully");
    }
    else if(check_operation_type(argc, argv) ==  e_decode)
    {
        printf("decode operation\n");
        read_and_validate_decode_args(argv, &decInfo,argc);
        decode_open_files(&decInfo);
        validate_decode_file(&decInfo);
        decode_file(&decInfo);
    }
    else
    {
        printf("Invalid syntax");
    }

    return 0;
}
