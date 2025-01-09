#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Malloc guaranteed to return a value or exit
void *xmalloc(size_t size) {
  void *retval = malloc(size);

  if(NULL == retval) {
    fprintf(stderr, "malloc() -> NULL\n");
    exit(EXIT_FAILURE);
  }
  return retval;
}

//realloc guaranteed to return a value or exit
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

//Load a file into memory 
char *load_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
      fprintf(stderr, "read_file() -> fopen() -> NULL\n");
      exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = xmalloc((*size) + 1);

    fread(buffer, 1, *size, file);
    fclose(file);

    buffer[*size] = '\0';
    
    return buffer;
}

int16_t index_from_id(const char *id) {
  int16_t return_value = id[0] - 'a';
  return_value = 26 * return_value + id[1] - 'a';
  return_value = 26 * return_value + id[2] - 'a';

  return return_value;
}

char *id_from_index(char *id, int16_t index) {
  id[2] = 'a' + index % 26;

  index /= 26;
  id[1] = 'a' + index % 26;

  index /= 26;
  id[0] = 'a' + index % 26;


  id[3] = '\0';
  
  return id;

}


typedef struct node_data_s *node_data;
struct node_data_s {
  int weight;
  int number_of_edges;
  int16_t *edges;
};

//Array index of all nodes
node_data *node_index = NULL;

node_data node_get_create(int16_t index) {
  if(NULL != node_index[index])
    return node_index[index];

  node_index[index] = xmalloc(sizeof(struct node_data_s));

  node_index[index]->weight = 0;
  node_index[index]->number_of_edges = 0;
  node_index[index]->edges = NULL;

  return node_index[index];
}

void node_add_edge(node_data node, int16_t index) {
  int edge_index = node->number_of_edges;
  node->number_of_edges += 1;
  node->edges = xrealloc(node->edges, node->number_of_edges * sizeof(int16_t));
  node->edges[edge_index] = index;
}

int num_edges = 0;


void nodes_connect(int16_t node1, int16_t node2) {
  node_data node1_data;
  node_data node2_data;

  node1_data = node_get_create(node1);
  node2_data = node_get_create(node2);

  node_add_edge(node1_data, node2);
  node_add_edge(node2_data, node1);
}

int main() {
  //Load file
  size_t loadsize;
  char *input = load_file("input", &loadsize);
  printf("Loaded %ld bytes\n", loadsize);

  //Prepare index of nodes
  int16_t index_size = index_from_id("zzz") + 1;
  node_index = xmalloc(index_size * sizeof(node_data));
  for(int i=0; i<index_size;i++)
    node_index[i] = NULL;

  //Fill index of nodes
  char *search_position = input;
  char id[4];
  while(1 == sscanf(search_position, "%3s:", id)) {
    int16_t node1 = index_from_id(id);

    search_position += 4;
    
    while(*search_position != '\n' &&
	  1 == sscanf(search_position, " %3s", id)) {
      
      int16_t node2 = index_from_id(id);

      nodes_connect(node1,node2);
      search_position += 4;
    }

    if(*search_position != '\n')
      printf("Newline expected but not found\n");
    
    
    search_position++;
    
    if(search_position[0] == '\0' ||
       search_position[1] == '\0') //Trailing newline 
      break;
  }
#if 1
  for(int i=0; i < index_size; i++)
    if(node_index[i] != NULL) {
      
      printf("%s:", id_from_index(id, i));

      node_data node = node_get_create(i);

      for(int j=0; j < node->number_of_edges;j++)
	printf(" %s",id_from_index(id, node->edges[j]));

      printf("\n");
    }
#endif

  //The problem is to divide the graph by cutting into two


  
  return EXIT_SUCCESS;
}
  num_edges++;

  
