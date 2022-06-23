
# LSB Based Image Steganography

Used to encode/decode the secret message and/or hide in another file


## Run Locally

Compile all the .c files in one go with GCC Compiler. 

```bash
    gcc *.c -o Stego
```

Encrypt the Image with the Secret text. Default image and secret text file 
is provided. Alternatively, own images can be used.

```bash
    Stego -e <filename>.bmp -st <secret_filename>.text
```

Decrypt the encrypted image

```bash
    Stego -d <filename>.bmp
```


