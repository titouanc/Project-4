#include <stdlib.h>
#include <string.h>
#include "pbm.h"
#include "barcode.h"
#include "file_foreach.h"

/*
 *************************************
 * main.c - ULg ID barcode generator *
 * ------                            *
 *************************************
 */

/*
 * Print usage on stdout
 */
static void usage(void);

/*
 * Callback invoked on each line of the input file
 * @pre : str is a valid NULL-terminated string (possibly empty)
 * @post: if str is a valid ULg ID, its bar code is written in [ULg ID].pbm
 *        Output an informative message on stdout for each non-empty line
 */
static bool renderUlgId(char *str);

int main(int argc, const char **argv){
  FILE *input;
  int i;
  if (argc < 2){
    usage();
    return 0;
  }
  
  for (i=1; i<argc; i++){
    input = (strcmp("-", argv[i]) == 0) ? stdin : fopen(argv[i], "r");
    if (! input){
      printf("Couldn't open file %s !\n", argv[i]);
      continue;
    }
    
    
    if (input == stdin) printf("Reading from stdin (CTRL+D to exit)\n");
    else printf("Reading file %s ...\n", argv[i]);
      
    fnforeach(input, 80, renderUlgId);
    if (input != stdin) fclose(input);
  }
  
  return EXIT_SUCCESS;
}

static bool renderUlgId(char *str){
  unsigned long long value;
  char filename[13] = {'\0'}; /* ULgID (%8d) + .pbm */
  char *error;
  PBM *barcode;
  
  if (strcmp(str, "\n") == 0)
    return true;
  
  value = (unsigned long long) strtoll(str, &error, 10);
  if (error == str || value >= 99999999)
    printf("%s doesn't look like an ULg ID\n", str);
  else {
    sprintf(filename, "%llu.pbm", value);
    barcode = Barcode_renderULL(value, 6);
    PBM_saveP1(barcode, filename, 10);
    PBM_destroy(barcode);
    printf("%s saved ", filename);
    if (value < 20000000) printf("(warning: not an ULg ID)");
    printf("\n");
  }
  
  return true;
}

static void usage(){
  printf("Usage: barcode FILE1 [ FILE2 [...] ] \n"
         "       where FILE is a path to a file which contain one ULg ID "
         "per line\n"
         "       if FILE is '-', reads from stdin\n");
}
