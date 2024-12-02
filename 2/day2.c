#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void *xmalloc(size_t size) {
  void *retval = malloc(size);

  if(NULL == retval) {
    fprintf(stderr, "malloc() -> NULL\n");
    exit(EXIT_FAILURE);
  }
  return retval;
}

void *xrealloc(void *pointer, size_t size) {
  void *retval = realloc(pointer, size);
  
  if(NULL == retval) {
    fprintf(stderr, "realloc() -> NULL\n");
    exit(EXIT_FAILURE);
  }
  return retval;
}

void xfree(void *pointer) {
  free(pointer);
}

char *xread_all(FILE *fp, size_t *size) {
  size_t capacity = 4096;
  size_t total_read = 0;
  char *buffer = xmalloc(capacity);

  while (1) {
    size_t bytes_read =
        fread(buffer + total_read, 1, capacity - total_read, fp);
    total_read += bytes_read;

    if (bytes_read < capacity - total_read) {
      if (feof(fp)) {
        break;
      } else if (ferror(stdin)) {
        fprintf(stderr, "xread() -> ferror() -> true\n");
        exit(EXIT_FAILURE);
      }
    }

    // Check if we need more space
    if (total_read == capacity) {
      capacity += 512;
      buffer = xrealloc(buffer, capacity);
    }
  }

  buffer = xrealloc(buffer, total_read + 1);
  buffer[total_read] = '\0';

  if (NULL != size)
    *size = total_read;
  
  return buffer;
}

char *xload_file(const char *filename, size_t *size) {

  FILE *file = fopen(filename, "rb");
    if (!file) {
      fprintf(stderr, "xread_file() -> fopen() -> NULL\n");
      exit(EXIT_FAILURE);
    }

    char *return_value = xread_all(file, size);

    fclose(file);

    return return_value;
}


char **xload_lines(const char *filename, uint64_t *number_lines) {

    char *file_content;
    size_t filesize;

    if(NULL != filename) {
      file_content = xload_file(filename, &filesize);
    }
    else {
      file_content = xread_all(stdin, &filesize);
    }
    
    // Get lines
    char **lines = NULL;
    uint64_t num_lines = 0;
    size_t size = 0;

    
    //Skip leading newlines
    char *current_line = file_content;
    while(*current_line == '\n')
      current_line++;

    
    char *end_of_line = strchr(current_line, '\n');

    if (NULL == end_of_line) {
      // Entire file is one string
      int line_length = strlen(current_line);
      if (line_length) {
        end_of_line = current_line + line_length;
      }
    }

    
    while (NULL != end_of_line) {
      *end_of_line = '\0';
      num_lines++;

      if (size < (num_lines * sizeof(char *))) {
        size += 1024;
        lines = xrealloc(lines, size);
      }
      lines[num_lines - 1] = current_line;

      current_line = end_of_line + 1;
      if (current_line > (file_content + filesize))
        break;

      end_of_line = strchr(current_line, '\n');
      if (NULL == end_of_line) {
        // One last line?
        int line_length = strlen(current_line);
        if (line_length) {
          end_of_line = current_line + line_length;
        }
      } else {
	//Eat newlines
        while(end_of_line[1] == '\n') {
          *end_of_line = '\0';
          end_of_line++;
	}
      }
    }

    
    if(0 == num_lines) {
      fprintf(stderr, "xload_lines() -> empty file\n");
      exit(EXIT_FAILURE);
    }

    
    //Terminate
    size = (num_lines + 1)*sizeof(char *);
    lines[num_lines] = NULL;

    
    //One memory location to rule them all
    size_t total_size = filesize + size;
    lines = xrealloc(lines, total_size);

    char *file_content2 = (char *) &(lines[num_lines + 2]);
    memcpy(file_content2, file_content, filesize);
    xfree(file_content);

    //Adjust pointers
    off_t adjustment = file_content2 - file_content;

    for(unsigned int i=0; i < num_lines; i++)
      lines[i] = lines[i] + adjustment;

    if(NULL != number_lines)
      *number_lines = num_lines;
    
    return lines; 
}

int *parse_line(char *line) {
  if(NULL == line)
    return NULL;

  int *temp = xmalloc(9 * sizeof(int));
  int num = 1; //1st is length

  while (1 == sscanf(line, "%d", temp + num)) {
      num++;
      line = strchr(line, ' ');
      if(NULL == line)
	break;
      line++; 
  }

  if(num < 5) { //Min amount of elements
    xfree(temp);
    return NULL;
  }
  
  temp[0] = num - 1;

  return temp;
}

int **parse_levels(char **input, int *num_levels) {
  size_t memsize = 0;
  int scanned = 0;
  int **return_value = NULL;
  int *temp;
  
  while (NULL != input && NULL != (temp = parse_line(*input))) {

      if (memsize < ((scanned + 1) * sizeof(int *))) {
        memsize += 1024;

        return_value = xrealloc(return_value, memsize);
      }

      return_value[scanned] = temp;
      scanned++;

      input++;
      }

  *num_levels = scanned;
  // Last elem not used

  return return_value;
}

int check_levels(int *levels, int number) {

  int sign[7] = { 0, 0, 0, 0, 0, 0, 0 };

  for(int i = 0; i < number - 1; i++) {
    int diff = levels[i] - levels[i+1];
    
    sign[i] = (diff > 0) ? 1 : -1;

    diff *= sign[i];

    if(diff < 1 || diff > 3)
      return 0;
  }

  //Check all signs are the same
  int sign_sum = 0;
  for(int i = 0 ; i < number - 1; i++)
    sign_sum += sign[i];

  sign_sum *= sign[0];

  if(sign_sum != number - 1)
    return 0;
      
  //Valid levels
  return 1;
}

void rotate_levels(int *levels, int num_levels) {
  int first = *levels;
  
  for(int i=0; i < (num_levels - 1);i++)
    levels[i] = levels[i+1];

  levels[num_levels - 1] = first;

  return; 
}

int check_levels_with_expception(int *levels, int number) {

  if(check_levels(levels,number))
    return 1;
  
  // Nope, check with exceptions
  int return_value = 0;
  int levels_temp[8];

  for (int i = 0; !return_value && i < number; i++) {
    for(int j = 0; j < number; j++)
      levels_temp[j] = levels[j];

    rotate_levels(levels_temp+i, number-i);

    return_value = check_levels(levels_temp, number - 1);
  }

  return return_value;
}

int main() {

  //Load
  uint64_t num_lines;
  char **lines = xload_lines("input", &num_lines);

#if 1
  for (unsigned int i = 0; i < num_lines; i++) {
    printf("%d: %s\n", i, lines[i]);
  }
#endif

  //parse
  int number_of_levels;
  int **levels = parse_levels(lines, &number_of_levels);
  xfree(lines);
  
  //Problem 1 
  int valid_levels = 0;
  for (int i = 0; i < number_of_levels; i++) {
    int *row = levels[i];

    if (check_levels(row + 1, *row))
      valid_levels++;
  }
  printf("Number of valid levels: %d\n", valid_levels);

  //Problem 2
  valid_levels = 0;
  for (int i = 0; i < number_of_levels; i++) {
    int *row = levels[i];

    if (check_levels_with_expception(row + 1, *row))
      valid_levels++;
  }
  printf("Number of valid levels w/ exception: %d\n", valid_levels);
  

  
  return EXIT_SUCCESS; //free everything :)
}
