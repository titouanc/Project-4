#include "pbm.h"
#include <assert.h>
#include <string.h>

/* PRIVATE HEADER */

/* Pixels are internally stored by block of 8 bits, aka char */
typedef unsigned char PixBlock;

/* Numbers of bits in a PixBlock. If some day we change PixBlock size... */
static const size_t PixBlock_bits = 8*sizeof(PixBlock);

/* An image is just a WxH PixBlock data structure */
struct PBM_t {
  size_t     width;
  size_t    height;
  PixBlock *pixmap;
};

/* Standard separators, by increasing precedence */
static const char PBM_separator[2] = {' ', '\n'};

/*
 * Convert a [x,y] position in image to a [offset_pixmap,offset_block] position
 * @pre : self is a valid PBM image, x<self.width, y<self.height
 *        offset_map != NULL, offset_block != NULL
 * @post: offset_pixmap: contains the offset of PixBlock containing pixel [x,y]
 *        offset_block : contains the offset of the bit in PixBlock which
 *                       represents pixel [x,y]
 */
static inline void PBM_offsets(PBM *self, size_t x, size_t y, 
                               size_t *offset_map, size_t *offset_block);

/*
 * @pre : offset<PixBlock_bits
 * @post: returns the bit at position offset in self
 */
static inline bool PixBlock_get(PixBlock self, size_t offset);

/*
 * @pre : self != NULL, offset<PixBlock_bits
 */
static inline void PixBlock_set(PixBlock *self, size_t offset, bool val);


/* PRIVATE IMPLEMENTATION */

static inline void PBM_offsets(PBM *self, 
                               size_t x, 
                               size_t y, 
                               size_t *offset_map, 
                               size_t *offset_block)
{
  size_t offset_pos;
  assert(self);
  assert(x<self->width && y<self->height);
  assert(offset_map != NULL && offset_block != NULL);
  
  offset_pos    = y*self->width + x;
  *offset_map   = offset_pos / PixBlock_bits;
  *offset_block = offset_pos % PixBlock_bits;
}

static inline bool PixBlock_get(PixBlock self, size_t offset){
  assert(offset<PixBlock_bits);
  return (bool) ((self >> offset) & 0x01);
}

static inline void PixBlock_set(PixBlock *self, size_t offset, bool val){
  assert(self);
  assert(offset<PixBlock_bits);
  if (val)
    *self |= (0x01<<offset);
  else
    *self &= (0xff^(0x01<<offset));
}


/* PUBLIC IMPLEMENTATION */

PBM *PBM_create(size_t width, size_t height){
  PBM *res;
  size_t area_len;
  assert(width>0 && height>0);
  
  res = malloc(sizeof(PBM));
  if (! res) return NULL;
  
  area_len = width*height;
  res->pixmap = malloc(area_len*sizeof(PixBlock));
  if (! res->pixmap){
    free(res);
    return NULL;
  }
  
  res->width  = width;
  res->height = height;
  memset(res->pixmap, 0, area_len*sizeof(PixBlock));
  return res;
}

void PBM_size(PBM *self, size_t *width, size_t *height){
  assert(self);
  if (width)  *width  = self->width;
  if (height) *height = self->height;
}

void PBM_destroy(PBM *self){
  assert(self);
  free(self->pixmap);
  free(self);
}

void PBM_writeP1(PBM *self, FILE *output, size_t scale){
  size_t x, x_scale, y, y_scale;
  size_t line_len = 0;
  bool val;
  assert(self);
  assert(scale>0);
  assert(output);
  
  fprintf(output, "P1%c%u%c%u%c", 
                  PBM_separator[1], 
                  (unsigned int) (self->width*scale), 
                  PBM_separator[0], 
                  (unsigned int) (self->height*scale), 
                  PBM_separator[1]);
  
  for (y=0; y<self->height; y++){
    for (y_scale=0; y_scale<scale; y_scale++){
      for (x=0; x<self->width; x++){
        val = PBM_get(self, x, y);
        for (x_scale=0; x_scale<scale; x_scale++){
          fprintf(output, "%1u%c", ((val) ? 1 : 0), PBM_separator[0]);
          line_len ++;
          if (line_len >= 34){ 
            fprintf(output, "%c", PBM_separator[1]);
            line_len = 0;
          }
        }
      }
      fprintf(output, "%c", PBM_separator[1]);
      line_len = 0;
    }
  }
}

bool PBM_saveP1(PBM *self, const char *filename, size_t scale){
  FILE *output = NULL;
  assert(filename && strlen(filename) > 0);
  
  output = fopen(filename, "w");
  if (! output) return false;
  
  PBM_writeP1(self, output, scale);
  fclose(output);
  return true;
}

/* Often used in readP1: sets error code in errptr to errval if error is
 * a non-null ptr, and returns retval
 */
#define setErrAndReturn(retval, errptr, errval) \
{if (errptr) *errptr=errval; return retval;}
PBM *PBM_readP1(FILE *handle, size_t scale, PBM_Error *error){
  char buffer[3] = {'\0'};
  size_t width=0, height=0;
  size_t x, y, x_scale, y_scale;
  PBM *img = NULL;
  int read_val;
  assert(handle);
  assert(scale>0);
  
  /* magic */
  if (fscanf(handle, "%2s", buffer) != 1)
    setErrAndReturn(NULL, error, PBM_FORMAT_ERROR);
  if (strcmp("P1", buffer) != 0)
    setErrAndReturn(NULL, error, PBM_MAGIC_ERROR);
  
  /* header */
  if (fscanf(handle, "%u %u", (unsigned int *) &width, 
             (unsigned int *) &height) != 2)
    setErrAndReturn(NULL, error, PBM_FORMAT_ERROR);
  
  width  /= scale;
  height /= scale;
  if (width<1 || height<1)
    setErrAndReturn(NULL, error, PBM_FORMAT_ERROR);
  
  img = PBM_create(width, height);
  if (! img)
    setErrAndReturn(NULL, error, PBM_MEMORY_ERROR);
  
  for (y=0; y<height; y++){
    for (x=0; x<width; x++){
      /* getting value */
      if (fscanf(handle, "%1d", &read_val) != 1)
        setErrAndReturn(img, error, PBM_LENGTH_ERROR);
      /* inserting value into PBM image */
      if (read_val) PBM_set(img, x, y, true);
      else          PBM_set(img, x, y, false);
      /* skipping unwanted columns */
      for (x_scale=1; x_scale<scale; x_scale++){
        if (fscanf(handle, "%1d", &read_val) != 1)
          setErrAndReturn(img, error, PBM_LENGTH_ERROR);
      }
    }
    /* skipping unwanted lines */
    for (y_scale=1; y_scale<scale; y_scale++){
      for (x=0; x<width*scale; x++){
        if (fscanf(handle, "%1d", &read_val) != 1)
          setErrAndReturn(img, error, PBM_LENGTH_ERROR);
      }
    }
  }
  
  setErrAndReturn(img, error, PBM_NO_ERROR);
}

PBM *PBM_openP1(const char *filename, size_t scale, PBM_Error *error){
  FILE *handle = NULL;
  PBM *img = NULL;
  assert(filename && strlen(filename) > 0);
  
  handle = fopen(filename, "r");
  if (! handle){
    if (error) *error = PBM_FILENOTFOUND;
    return NULL;
  }
  
  img = PBM_readP1(handle, scale, error);
  fclose(handle);
  
  return img;
}

bool PBM_get(PBM *self, size_t col, size_t row){
  size_t offset_map, offset_block;
  PBM_offsets(self, col, row, &offset_map, &offset_block);
  return PixBlock_get(self->pixmap[offset_map], offset_block);
}

void PBM_set(PBM *self, size_t col, size_t row, bool val){
  size_t offset_map, offset_block;
  PBM_offsets(self, col, row, &offset_map, &offset_block);
  PixBlock_set(&(self->pixmap[offset_map]), offset_block, val);
}

void PBM_invert(PBM *self, size_t col, size_t row){
  PBM_set(self, col, row, ! PBM_get(self, col, row));
}
