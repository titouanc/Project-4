#ifndef DEFINE_PBMTTY_HEADER
#define DEFINE_PBMTTY_HEADER

#include "pbm.h"
#include <stdio.h>

/*
 * @pre : img is a valid PBM image, output is a file opened in write mode, 
 *        typically an ANSI terminal which renders colors
 * @post: img is written to output with special color control characters
 */
void PBM_writeTTY(PBM *img, FILE *output);

#endif
