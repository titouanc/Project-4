# INFO0030 - Project 4 (Barcode)
## by Titouan CHRISTOPHE (20111001)

The first goal of this project is to build 2D barcodes from numeric values.

The second goal of this project is to make a tool that checks the validity of
a barcode, and eventually correct it if there's only one error in it. There's
already a command line tool doing this stuff, _checkbar_, which is built with
  
  make [all]

or

  make checkbar

Basically, errors are detected from parity row (last row) and column (last 
column), and a bit (bottom right) which ensure those lines are correct too. 

The function Barcode_validateChecksum (from barcode.h) handle this verification. 
Just pass a PBM image, in the same format as outputed by Barcode_renderULL. The
image _must be at least_ 2x2 (1x1 data section) and _at most_ 9x9 (8x8 data 
section). 

Images could be loaded from PBM "P1" (ASCII) files, using PBM_openP1. For this 
project, barcodes image are written with an increased scale, though this is done
only at write-time. Counterpart, the loading functions also have the ability to 
reduce image at load time. Thus, a typical call sequence to check a barcode 
validity would be:

  /* opening image.pbm, reducing its scale by 10, and getting eventual
  loading errors in errcode (see PBM.h for error codes definitions).
  Note that it isn't required to pass an error verification variable,
  if you don't care, just pass NULL */
  image = PBM_openP1("image.pbm", 10, &errcode);
  
  /* If a fatal error occured, no image were returned; just get out */
  if (! image) return;
  
  /* Here's the trick ! */
  switch (Barcode_validateChecksum(image)){
    case -1: 
      /* barcode couldn't be corrected: more than 1 error or unknown scenario */ 
      break;
    case  0: 
      /* barcode is valid */
      break;
    case  1:
      /* barcode has been corrected by the function */
      break;
  }
  
  /* Don't forget to free memory, especially in long-run programs */
  PBM_destroy(image);


