#include <assert.h>
#include "barcode.h"
#include <stdio.h>

/* PRIVATE HEADER */

/*
 * Compute checksum for a barcode. Checksum row and col are arrays of 8bits 
 * (aka char). Bit is the last checksum bit, in the bottom-right cell.
 * @pre : barcode is a valid Barcode, col, row and bit are valid pointers
 * @post: col, row and bits are filled with the corresponding parity bits.
 *        col contain the bits for the last column, row for the last row.
 *        To know if column[i] (first column is 0) is odd, test whether
 *        (row>>i)&1 != 0
 */
static void Barcode_mkChecksum(PBM *barcode, unsigned char *col, 
  unsigned char *row, bool *bit);

/*
 * Draw checksum in a barcode, following the same format as Barcode_mkChecksum
 * @pre : barcode is a valid Barcode
 * @post: checksum drawn in barcode
 */
static void Barcode_drawChecksum(PBM *barcode, unsigned char col, 
  unsigned char row, bool bit);

/*
 * Reads an existing checksum in a barcode, following the same format as 
 * Barcode_mkChecksum. Useful to compare computed checksum and drawn checksum
 * @pre : barcode is a valid Barcode, col, row and bit are valid pointers
 * @post: col, row and bit are filled with appropriate values (see mkChecksum)
 */
static void Barcode_readChecksum(PBM *barcode, unsigned char *col,
  unsigned char *row, bool *bit);

/*
 * Helper function to make & draw the checksum in a barcode
 */
static inline void Barcode_renderChecksum(PBM *barcode);

/*
 * Compute checksum bit (bottom-right in image) according to col and row
 * @pre : / (col and row typically built with Barcode_mkChecksum)
 * @post: return 1 if col and row are odd, 
 *               0 if col and row are even, 
 *              -1 if col and row have different parities
 */
static int Barcode_mkCheckBit(unsigned char col, unsigned char row);


/* PRIVATE IMPLEMENTATION */

static int Barcode_mkCheckBit(unsigned char col, unsigned char row){
  int sum_col=0, sum_row=0;
  size_t i;
  
  for (i=0; i<8; i++){
    if ((col>>i)&0x01) sum_col ++;
    if ((row>>i)&0x01) sum_row ++;
  }
  
  return (sum_col != sum_row) ? -1 : sum_col%2;
}

static void Barcode_mkChecksum(PBM *barcode, unsigned char *col, 
                               unsigned char *row, bool *bit)
{
  size_t size, i, j;
  unsigned int sum_row=0, sum_col=0;
  assert(col);
  assert(row);
  assert(bit);
  
  assert(barcode);
  PBM_size(barcode, &i, &j);
  assert(i == j);
  assert(2<=i && i<=9);
  
  size = i-1;
  *col = *row = 0;
  
  for (i=0; i<size; i++){
    sum_row = sum_col = 0;
    for (j=0; j<size; j++){
      if (PBM_get(barcode, i, j)) sum_col++;
      if (PBM_get(barcode, j, i)) sum_row++;
    }
    if (sum_row%2) *col |= (0x01 << i);
    if (sum_col%2) *row |= (0x01 << i);
  }
  *bit = Barcode_mkCheckBit(*col, *row);
}

static void Barcode_drawChecksum(PBM *barcode, unsigned char col, 
                                 unsigned char row, bool bit)
{
  size_t size, width, height, i;
  assert(barcode);
  PBM_size(barcode, &width, &height);
  assert(width == height);
  assert(2<=width && width<=9);
  
  size = width - 1;
  
  for (i=0; i<size; i++){
    PBM_set(barcode, size, i, (bool) ((col>>i)&1));
    PBM_set(barcode, i, size, (bool) ((row>>i)&1));
  }
  PBM_set(barcode, size, size, bit);
}

static void Barcode_readChecksum(PBM *barcode, unsigned char *col,
                                 unsigned char *row, bool *bit)
{
  size_t width, height, size, i;
  assert(barcode);
  PBM_size(barcode, &width, &height);
  assert(width == height);
  assert(2<=width && width<=9);
  
  size = width - 1;
  *bit = PBM_get(barcode, size, size);
  
  *row = *col = 0;
  for (i=0; i<size; i++){
    if (PBM_get(barcode, i, size)) *row |= (0x01 << i);
    if (PBM_get(barcode, size, i)) *col |= (0x01 << i);
  }
}

static inline void Barcode_renderChecksum(PBM *barcode){
  unsigned char col, row;
  bool bit;
  assert(barcode);
  
  Barcode_mkChecksum(barcode, &col, &row, &bit);
  Barcode_drawChecksum(barcode, col, row, bit);
}

/* PUBLIC IMPLEMENTATION */

PBM *Barcode_renderULL(unsigned long long value, size_t size){
  PBM *img = NULL;
  size_t i;
  /* 0 make nonsense, no native data type for over 64 (8x8) bits */
  assert(size<=8 && size > 0);
  /* value mustn't overflow the barcode capacity */
  if (size < 8)
    assert(value < (((unsigned long long) 1) << (size*size)));
  else
    assert(value <= 0xffffffffffffffff);
  
  img = PBM_create(size+1, size+1);
  if (! img)
    return NULL;
    
  for (i=0; i<size*size; i++)
    PBM_set(img, i%size, i/size, (bool) ((value>>i) & 1));
  
  //Barcode_mkChecksum(img);
  Barcode_renderChecksum(img);
  
  return img;
}

int Barcode_validateChecksum(PBM *barcode){
  unsigned char row_img=0, col_img=0; /* checksum in image */
  unsigned char row_computed=0, col_computed; /* checksum for data zone */
  unsigned char row_err=0, col_err=0; /* error mask */
  bool          bit_img=false, bit_computed=false; /* checksum bits */
  int           bit_img_computed=0; /* parity bit computed from img csums */
  size_t        size=0, i=0, wrong_bit_x=0, wrong_bit_y=0;
  int           col_err_count=0, row_err_count=0; /* errors in checksum lines */
  assert(barcode);
  
  /* Reading existing checksum in barcode */
  Barcode_readChecksum(barcode, &col_img, &row_img, &bit_img);
  /* Computing checksum for datazone */
  Barcode_mkChecksum(barcode, &col_computed, &row_computed, &bit_computed);
  
  /* No error in barcode */
  if (col_img == col_computed && 
      row_img == row_computed && 
      bit_img == bit_computed)
    return 0;
  
  /* creating error mask */
  col_err = col_img ^ col_computed;
  row_err = row_img ^ row_computed;
  
  /* Setting up for loops */
  PBM_size(barcode, &size, NULL);
  size--;
  
  /* Checking for errors number and position in error mask */
  for (i=0; i<size; i++){
    if ((col_err>>i)&0x01){
      col_err_count++;
      wrong_bit_y = i;
    }
    if ((row_err>>i)&0x01){
      row_err_count++;
      wrong_bit_x = i;
    }
  }
  
  /* Computing parity bit according to image rows and cols checksum */
  bit_img_computed = Barcode_mkCheckBit(col_img, row_img);
  
  /* 1 data bit inversion */
  if (col_err_count == 1 && row_err_count == 1 && bit_img_computed == bit_img){
    PBM_invert(barcode, wrong_bit_x, wrong_bit_y);
    return 1;
  } 
  
  /* 1 checksum col bit inversion */
  if (col_err_count == 1 && row_err_count == 0 && bit_img_computed != bit_img){
    PBM_invert(barcode, size, wrong_bit_y);
    return 1;
  }
  
  /* 1 checksum row bit inversion */
  if (col_err_count == 0 && row_err_count == 1 && bit_img_computed != bit_img){
    PBM_invert(barcode, wrong_bit_x, size);
    return 1;
  } 
  
  /* Parity bit inversion */
  if (col_err_count == 0 && row_err_count == 0 && bit_img_computed != bit_img){
    PBM_invert(barcode, size, size);
    return 1;  
  }
  
  return -1;
}
