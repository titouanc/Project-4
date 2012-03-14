#ifndef DEFINE_BARCODE_HEADER
#define DEFINE_BARCODE_HEADER

/*
 ************************************************
 * barcode.h - Barcode quick rendering with PBM *
 * ---------                                    *
 * Renders numbers in project's expected format *
 * A valid barcode is a square PBM image which  *
 * size is between 2x2 and 9x9 pixels.          *
 ************************************************
 */

#include <stdlib.h>
#include <stdbool.h>
#include "pbm.h"

/*
 * @pre : size>0, size<8, value<(2**(size*size))
 * @post: returns a PBM image representing the barcode, 
 *        or NULL if an error occured. The returned image has 
 *        (size+1)x(size+1) pixels, the added row and column are checksums.
 */
PBM *Barcode_renderULL(unsigned long long value, size_t size);

/*
 * Attempt to rectify a barcode if it contains at most one error
 * @pre : barcode is a valid barcode containing at most 1 error
 * @post: if function returns  0, barcode is valid and no change were made
 *        if function returns  1, barcode had 1 error and has been corrected
 *        if function returns -1, barcode is not rectifiable, no change made
 */
int Barcode_validateChecksum(PBM *barcode);

#endif
