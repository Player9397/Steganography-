COMMAND NAME :: ./a.out ( compiled default output )
DESCRIPTION  :: Used to encode/decode the secret message and/or hide in another file 
USAGE        :: 
        
        -e   -->  encode followed by file_name of the file to be encoded
        -d   -->  decode followed by the file_name to be decoded 
        -st  -->  denotes the secret text file

        Encode Operation
        ./a.out -e <filename>.bmp -st <secret_filename>.text
                *** It generates a new .bmp file (Stego.bmp as Default) where the secret text is encoded.
        
                Secret text format
                <Magic String #* (16Bytes)>
                <extension size(32Bytes)>
                <extension (depends on extension size)>
                <secrettext size( 32Bytes )>
                <secret data>        

        Decode Operation
        ./a.out -d <filename>.bmp
                ****** The secret message is displayed ******


