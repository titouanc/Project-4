#ifndef DEFINE_PBM_HEADER
#define DEFINE_PBM_HEADER

/*
 **********************************************************
 * pbm.h - Portable Bit Map image (binary pixmap)         *
 * -----                                                  *
 * Interface for a minimal PBM implementation             *
 * Conform to http://netpbm.sourceforge.net/doc/pbm.html, *
 * except that it doesn't ignore line starting with "#"   *
 * when reading files.                                    *
 **********************************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Represents a PBM image with this coordinate system:
 *         0,0 ... width-1,0
 *         ...     ...
 *  0,height-1 ... width-1,height-1
 */
typedef struct PBM_t PBM;

/* Error codes returned by load functions */
typedef enum {
  PBM_NO_ERROR    , /* No error happened during reading */
  PBM_MAGIC_ERROR , /* Unexpected magic number */
  PBM_FORMAT_ERROR, /* Unexpected format */
  PBM_LENGTH_ERROR, /* The raster section is shorter than WxH */
  PBM_MEMORY_ERROR, /* No more memory could be allocated */
  PBM_FILENOTFOUND  /* File not found for PBM_openP1 */
} PBM_Error;

/*
 * @pre : width > 0, height > 0
 * @post: return a new properly initialised PBM image
 *        or NULL if an error occured
 */
PBM *PBM_create(size_t width, size_t height);

/*
 * @pre : self is a valid PBM image
 * @post: *width = self.width, *height = self.height.
 * @rem : width or height could be NULL
 */
void PBM_size(PBM *self, size_t *width, size_t *height);

/*
 * @pre : self is a valid PBM image
 * @post: memory freed for self
 */
void PBM_destroy(PBM *self);

/*
 * @pre : self is a valid PBM image, x<self.width, y<self.height
 * @post: returns the value at self[col,row]
 */
bool PBM_get(PBM *self, size_t col, size_t row);

/*
 * @pre : self is a valid PBM image, x<self.width, y<self.height
 * @post: set the value at self[col,row]
 */
void PBM_set(PBM *self, size_t col, size_t row, bool val);

/*
 * @pre : self is a valid PBM image, x<self.width, y<self.height
 * @post: invert the value at self[col,row]
 */
void PBM_invert(PBM *self, size_t col, size_t row);

/*
 * @pre : self is a valid PBM image, output is opened in write mode, scale>0
 * @post: self is written expanded by scale in output, 
 *        according to PBM "P1" (ASCII) format
 */
void PBM_writeP1(PBM *self, FILE *output, size_t scale);

/*
 * Reads a file in the P1 format. If an error occurs, its code is placed in 
 * error (optional). See PBM_Error definition for their significations
 * If scale > 1, reads 1 pixel then skip scale-1 columns and lines
 * (read-time resize)
 * @pre : handle is an opened file, scale>0, error a valid pointer or NULL
 * @post: returns a new properly initialised PBM image, or NULL if a fatal
 *        error occurs. If a PBM_LENGTH_ERROR occurs, missing bits will be
 *        filled with zeros, and image will be returned.
 */
PBM *PBM_readP1(FILE *handle, size_t scale, PBM_Error *error);

/*
 * @pre : same as PBM_writeP1 except that we pass a file path instead of
 *        a file pointer. Filename is a valid C string, filename.length>0
 * @post: return true, or false if output file couldn't be opened
 */
bool PBM_saveP1(PBM *self, const char *filename, size_t scale);

/*
 * @pre : same as PBM_readP1 except that we pass a file path instead of
 *        a file pointer. Filename is a valid C string, filename.length>0
 * @post: same as PBM_readP1
 */
PBM *PBM_openP1(const char *filename, size_t scale, PBM_Error *error);

#endif
