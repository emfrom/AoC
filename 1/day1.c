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

char *load_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
      fprintf(stderr, "read_file() -> fopen() -> NULL\n");
      exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = xmalloc(*size);

    fread(buffer, 1, *size, file);
    fclose(file);
    
    return buffer;
}



int convert_input(char *data, int **vectorA, int **vectorB) {

  int *vectora = NULL;
  int *vectorb = NULL;
  size_t size = 0;
  int scanned = 0;
  int a,b;

  while(2 == sscanf(data,"%d %d",&a, &b)) {

      if(size < ((scanned + 1) * sizeof(int))) {
	size += 512;
	
	vectora = xrealloc(vectora, size);
	vectorb = xrealloc(vectorb, size);
      }

      vectora[scanned] = a;
      vectorb[scanned] = b;

      scanned++;

      data = strchr(data,'\n');
      if(NULL == data) {
	break;
      }
      data++; 
    }

  *vectorA = vectora;
  *vectorB = vectorb;

  return scanned;
}


// Comparison for qsort
int compare_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

//Find int in sorted int array, binary search 
int find_int(int *array, int array_size, int target) {
    int left = 0;
    int right = array_size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (array[mid] == target) {
            return mid; 
        } else if (array[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1; 
        }
    }

    return -1; //Not found
}

int count_occurences(int *array, int array_size, int target) {
  int position = find_int(array, array_size, target);
  int occurences = 0;
  
  //Not found
  if(position < 0)
    return 0;

  //Bsearch might return first or last element
  //Find first
  while(position > 0 &&
	array[position - 1] == target)
    position--;

  //Count
  while(array[position] == target) {
    position++;
    occurences++;
  }

  return occurences;
}


int main() {
  //Load input
  size_t loadsize;
  char *input;
  input = load_file("input", &loadsize);
  printf("%ld bytes read\n", loadsize);
#if 0
  fwrite(input, loadsize, 1, stdout);
  fputc('\n',stdout);
#endif

  
  //Scan 
  uint32_t scanned;
  int *vectorA, *vectorB;
  scanned = convert_input(input, &vectorA, &vectorB);
  printf("%d elements converted\n", scanned);
#if 0
  for(int i = 0; i < scanned; i++)
    printf("%d: %d %d\n",
	   i,
	   vectorA[i],
	   vectorB[i]);
#endif
  
  //Sort 
  qsort(vectorA, scanned, sizeof(int), compare_int);
  qsort(vectorB, scanned, sizeof(int), compare_int);
#if 0
  for(int i = 0; i < scanned; i++)
    printf("%d: %d %d\n",
	   i,
	   vectorA[i],
	   vectorB[i]);
#endif

  //Reduce (absolute sum of differences)
  uint64_t sum_of_distances = 0;
  for(int i = 0; i < scanned; i++) 
    sum_of_distances += ( vectorA[i] > vectorB[i] ?
			  vectorA[i] - vectorB[i] :
			  vectorB[i] - vectorA[i] );
  printf("The sum of the distances is: %ld\n", sum_of_distances);
  

  //Count occurences
  //Need not be unique in vectorA 
  int *occurences = xmalloc(scanned * sizeof(int));
  for(int i=0; i < scanned; i++) 
    occurences[i] = count_occurences(vectorB, scanned, vectorA[i]);

#if 0
  for (int i = 0; i < scanned; i++)
    if (occurences[i])
           printf("%d: %d\n", vectorA[i], occurences[i]);
  //Note: there arent really that many
#endif

  //Reduce (value * occurences)
  uint64_t similarity_score = 0;
  for (int i = 0; i < scanned; i++)
    if (occurences[i])
      similarity_score += occurences[i]*vectorA[i];
  
  printf("The similarity score is: %ld\n", similarity_score);


  return EXIT_SUCCESS;
}

