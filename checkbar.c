#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "barcode.h"

/*
 ***************************************
 * checkbar.c - ULg ID barcode checker *
 * ----------                          *
 ***************************************
 */

/*
 * Try to rectify a barcode. If successful, save correct version.
 * @pre : filename is a valid non-empty C string
 * @post: if image located at filename is an invalid barcode with one error, 
 *        it is corrected and saved with '-rectified' suffix. If it has no
 *        error, or more than one error, nothing is done.
 *        Output an informative message on stdout
 */
void quickCheck(char *filename);

int main(int argc, char **argv){
  int i;
  
  if (argc > 1){
    for (i=1; i<argc; i++)
      quickCheck(argv[i]);
  } else {
    printf("Usage: checkbar FILE1 [ FILE2 [...] ]\n"
           "       where FILE is a path to a PBM file in the same format as "
           "outputed by the barcode program.\n");
  }
  
  return EXIT_SUCCESS;
}

void quickCheck(char *filename){
  PBM_Error read_error;
  char  *new_filename=NULL;
  size_t filename_len=0;
  PBM *barcode = NULL;
  
  assert(filename);
  filename_len = strlen(filename);
  assert(filename_len > 0);
  
  barcode = PBM_openP1(filename, 10, &read_error);
  
  printf("Checking %s... ", filename);
  
  if (read_error == PBM_NO_ERROR){
    switch (Barcode_validateChecksum(barcode)){
      case 0:  printf("valid.\n"); break;
      case 1:  
        printf("rectified. "); 
        new_filename = malloc((filename_len+11)*sizeof(char));
        if (new_filename){
          strcpy(new_filename, filename);
          strcpy(&(new_filename[filename_len-4]), "-rectified.pbm");
          if (PBM_saveP1(barcode, new_filename, 10))
            printf("Saved as %s", new_filename);
          else
            printf("Error when saving as %s", new_filename);
          free(new_filename);
        }
        printf("\n");
        break;
      default: printf("unable to rectify !!!\n"); break;
    }
  } else {
    printf("Error when reading file: ");
    switch (read_error){
      case PBM_MAGIC_ERROR: 
        printf("unknow magic number"); break;
      case PBM_FORMAT_ERROR:
        printf("unexpected format"); break;
      case PBM_LENGTH_ERROR:
        printf("length error"); break;
      case PBM_MEMORY_ERROR:
        printf("not enough available memory"); break;
      case PBM_FILENOTFOUND:
        printf("file not found"); break;
      default : break;
    }
    printf("\n");
  }
  
  if (barcode) PBM_destroy(barcode);
}
