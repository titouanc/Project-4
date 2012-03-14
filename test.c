#include <stdio.h>
#include "pbm.h"
#include "barcode.h"

void gentleTest(bool expectation, const char *msg);

int main(int argc, const char **argv){
  PBM *barcode = Barcode_renderULL(20111001, 6);
  
  if (Barcode_validateChecksum(barcode) != 0)
    printf("Test de creation valide foireux !\n");
  
  PBM_writeTTY(barcode, stdout); printf("\n\n");
  PBM_invert(barcode, 6, 6);
  PBM_writeTTY(barcode, stdout);
  if (Barcode_validateChecksum(barcode) != 1)
    printf("Test de correction checksum bit foireux !\n");
  
  PBM_writeTTY(barcode, stdout);
  
  PBM_destroy(barcode);
  return 0;
}
