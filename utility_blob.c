#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void *xmalloc(size_t size)
{
    void *retval = malloc(size);

    if (NULL == retval) {
        fprintf(stderr, "malloc() -> NULL\n");
        exit(EXIT_FAILURE);
    }
    return retval;
}

void *xrealloc(void *pointer, size_t size)
{
    void *retval = realloc(pointer, size);

    if (NULL == retval) {
        fprintf(stderr, "realloc() -> NULL\n");
        exit(EXIT_FAILURE);
    }
    return retval;
}

void xfree(void *pointer)
{
    free(pointer);
}

char *xread_all(FILE *fp, size_t *size)
{
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

char *xload_file(const char *filename, size_t *size)
{

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "xread_file() -> fopen() -> NULL\n");
        exit(EXIT_FAILURE);
    }

    char *return_value = xread_all(file, size);

    fclose(file);

    return return_value;
}


char **xload_lines(const char *filename, int *number_lines)
{

    char *file_content;
    size_t filesize;

    if (NULL != filename) {
        file_content = xload_file(filename, &filesize);
    } else {
        file_content = xread_all(stdin, &filesize);
    }

    // Get lines
    char **lines = NULL;
    uint64_t num_lines = 0;
    size_t size = 0;


    //Skip leading newlines
    char *current_line = file_content;
    while (*current_line == '\n')
        current_line++;


    char *end_of_line = strchr(current_line, '\n');

    if (NULL == end_of_line) {
        // Entire file is one line
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
            while (end_of_line[1] == '\n') {
                *end_of_line = '\0';
                end_of_line++;
            }
        }
    }


    if (0 == num_lines) {
        fprintf(stderr, "xload_lines() -> empty file\n");
        exit(EXIT_FAILURE);
    }
    //Terminate
    size = (num_lines + 1) * sizeof(char *);
    lines[num_lines] = NULL;


    //One memory location to rule them all
    size_t total_size = filesize + size;
    lines = xrealloc(lines, total_size);

    char *file_content2 = (char *) &(lines[num_lines + 1]);
    memcpy(file_content2, file_content, filesize);
    xfree(file_content);

    //Adjust pointers
    off_t adjustment = file_content2 - file_content;

    for (unsigned int i = 0; i < num_lines; i++)
        lines[i] = lines[i] + adjustment;


    if (NULL != number_lines)
        *number_lines = num_lines;

    return lines;
}

//for stuff loaded with xload_lines
char **xdup_lines(char **lines)
{
    char **temp = lines;
    int end = 0;

    while (temp[end] != NULL)
        end++;
    end--;

    char *last_line = temp[end];
    while (*last_line != '\0')
        last_line++;
    last_line++;

    size_t size = last_line - (char *) lines;

    temp = xmalloc(size);
    memcpy(temp, lines, size);

    //Adjust pointers
    off_t adjustment = temp - lines;

    for (int i = 0; temp[i] != NULL; i++)
        temp[i] = temp[i] + adjustment;

    return temp;
}

#include <regex.h>

char *xregex_search(const char *haystack, const char *pattern)
{
    regex_t regex;
    regmatch_t match;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "xregex_search() -> Regex compilation error\n");
        exit(EXIT_FAILURE);
    }

    if (regexec(&regex, haystack, 1, &match, 0) == 0) {
        regfree(&regex);
        return (char *) haystack + match.rm_so;
    }

    regfree(&regex);
    return NULL;
}


typedef struct linked_list_s *linked_list;
struct linked_list_s {
    void *data;
    linked_list next;
};

linked_list linked_list_prepend(linked_list head, void *data)
{
    linked_list new_node =
        (linked_list) xmalloc(sizeof(struct linked_list_s));
    new_node->data = data;
    new_node->next = head;
    return new_node;
}

#if REALLY_NECESSARY
linked_list linked_list_append(linked_list head, void *data)
{
    linked_list new_node =
        (linked_list) xmalloc(sizeof(struct linked_list_s));
    new_node->data = data;
    new_node->next = NULL;

    if (!head)
        return new_node;

    linked_list current = head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_node;
    return head;
}
#endif


linked_list linked_list_duplicate(linked_list head)
{
    if (!head) {
        return NULL;
    }

    linked_list new_head =
        (linked_list) xmalloc(sizeof(struct linked_list_s));
    new_head->data = head->data;
    new_head->next = NULL;

    linked_list current = head->next;
    linked_list new_current = new_head;

    while (current) {
        linked_list new_node =
            (linked_list) xmalloc(sizeof(struct linked_list_s));
        new_node->data = current->data; // Shallow copy
        new_node->next = NULL;

        new_current->next = new_node;
        new_current = new_node;
        current = current->next;
    }

    return new_head;
}

linked_list linked_list_destroy(linked_list head,
                                void (*destroy_data)(void *data))
{
    linked_list current = head;
    while (current) {
        linked_list next = current->next;
        if (NULL != destroy_data)
            destroy_data(current->data);
        xfree(current);
        current = next;
    }

    return NULL;
}


struct field_s {
    int xsize;
    int ysize;
    char **content;
};
typedef struct field_s *field;


field field_create()
{
    field field_data;
    int n_lines;

    field_data = xmalloc(sizeof(struct field_s));

    field_data->content = xload_lines("input", &n_lines);       //For AoC
    field_data->ysize = n_lines;
    field_data->xsize = strlen(field_data->content[0]);

    return field_data;
}


field field_copy(field copy, field original)
{
    for (int i = 0; i < copy->ysize; i++)
        strncpy(copy->content[i], original->content[i], copy->xsize + 1);

    return copy;
}

field field_duplicate(field original)
{
    field new = xmalloc(sizeof(struct field_s));

    new->xsize = original->xsize;
    new->ysize = original->ysize;
    new->content = xdup_lines(original->content);

    return new;
}

int field_inbounds(field playarea, int x, int y)
{
    return !(x < 0 || y < 0 || x >= playarea->xsize
             || y >= playarea->ysize);
}


void field_destroy(field playarea)
{
    xfree(playarea->content);
    xfree(playarea);

}

char field_get(field area, int x, int y)
{
    return area->content[y][x];
}

void field_set(field area, int x, int y, char c)
{
    area->content[y][x] = c;
}

field field_soliton = NULL;

void field_soliton_destroy()
{
    if (NULL != field_soliton)
        field_destroy(field_soliton);

    field_soliton = NULL;
}

field field_soliton_get()
{
    if (NULL == field_soliton) {
        field_soliton = field_create();
        atexit(field_soliton_destroy);
    }
    return field_soliton;
}

void field_for_all(field area, void (*function)(int, int, void *),
                   void *data)
{
    for (int y = 0; y < area->ysize; y++)
        for (int x = 0; x < area->xsize; x++)
            function(x, y, data);
}

void field_print(field area)
{
    for (int i = 0; i < area->ysize; i++)
        printf("%s\n", area->content[i]);
}


typedef struct cordinate_s *cordinate;
struct cordinate_s {
    int x;
    int y;

    cordinate next;
};

cordinate cordinate_create(int x, int y)
{
    cordinate cord = xmalloc(sizeof(struct cordinate_s));

    cord->x = x;
    cord->y = y;

    cord->next = NULL;

    return cord;
}

cordinate cordinate_chain(cordinate first, cordinate second)
{
    cordinate temp = first;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = second;

#if 0
    temp = first;
    int count = 0;
    while (temp != NULL) {
        temp = temp->next;
        count++;
    }
    printf("Chained %d coordinates\n", count);
#endif

    return first;
}
