#include "pbm_tty.h"
#include <assert.h>

static char PBM_TTY_COLORS[2] = {7, 0};

void PBM_writeTTY(PBM *img, FILE *output){
  size_t width, height, x, y;
  assert(img);
  assert(output);
  
  PBM_size(img, &width, &height);
  for (y=0; y<height; y++){
    for (x=0; x<width; x++){
      fprintf(output, "\033[4%1dm  ", PBM_TTY_COLORS[PBM_get(img, x, y)]);
    }
    fprintf(output, "\n");
  }
  fprintf(output, "\033[0m");
}
