#include "file_foreach.h"
#include <stdlib.h>
#include <assert.h>

bool fnforeach(FILE *self, int line_max_len, bool (*callback)(char *line)){
  char *line = NULL;
  assert(self);
  assert(line_max_len > 0);
  assert(callback);
  
  line = malloc(line_max_len*sizeof(char));
  if (! line)
    return false;
  
  while (fgets(line, line_max_len, self)){
    if (! callback(line)){
      free(line);
      return false;
    }
  }
  
  free(line);
  return true;
}
