#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "../utility_blob.c"

// Notes day 9
//
// I optimized too early today. I had my eye on the never doing anything twice
// and shrinking the area where I was looking for empty spaces to fill
//
// Overall pretty happy with it
//
// edit: More than happy with it actually, P2 solution is O(n)
//       This is since both helper functions only ever visit a location k times,
//       k < 10
//       Moving and managing the hints is also k < 10 each
//       ergo, O(n) 
//       
#define FILE 1
#define FREESPACE -1

int *compact_to_id(char *compact, int *disksize)
{
    size_t size = 0;
    int n_blocks = 0;
    int *blocks = NULL;
    int file_or_space = FILE;
    int current_id = 0;

    while (isdigit(*compact)) {
        int length = *compact - '0';
        compact++;

        if (size < (n_blocks + length) * sizeof(int)) {
            size += 1024;
            blocks = realloc(blocks, size);
        }

        for (int i = n_blocks; i < n_blocks + length; i++) {
            if (file_or_space == FILE)
                blocks[i] = current_id;
            else
                blocks[i] = FREESPACE;
        }

        n_blocks += length;

        if (FILE == file_or_space) {
            current_id++;
            file_or_space = FREESPACE;
        } else
            file_or_space = FILE;
    }

    if (disksize)
        *disksize = n_blocks;

#if 0
    for (int i = 0; i < n_blocks; i++) {
        if (blocks[i] < 0)
            printf(".");
        else
            printf("%d", blocks[i]);
    }
#endif

    return blocks;
}

int *defragment_simple(int *blocks, int n_blocks)
{
    int *end = blocks + n_blocks - 1;
    int *work = blocks;

    for (;;) {
        while (*end == FREESPACE)
            --end;

        while (*work != FREESPACE)
            ++work;

        if (end < work)
            break;

        *work = *end;
        *end = FREESPACE;
    }

    return blocks;
}

int *find_free_block(int *blocks, int *end, int len)
{
    for (;;) {
        int temp_len;
        temp_len = 0;

        while (blocks < end && *blocks != FREESPACE)
            ++blocks;

        if (blocks >= end)
            return end;

        int *temp = blocks;

        while (blocks < end && *blocks == FREESPACE) {
            temp_len++;

            if (temp_len == len)
                return temp;

            blocks++;
        }
    }
}

int find_last_id(int *end)
{
    while (*end == FREESPACE)
        --end;

    return *end;
}

int *find_id_backwards(int id, int **end, int *len)
{

    int *pos = *end;

    while (*pos != id)
        pos--;

    *len = 0;

    while (*pos == id) {
        *len += 1;
        pos--;
    }

    pos++;
    *end = pos;

    return pos;
}

void move_block(int *dest, int *source, int len)
{
    for (int i = 0; i < len; i++) {
        dest[i] = source[i];
        source[i] = FREESPACE;
    }
}

int *defragment(int *blocks, int n_blocks)
{
    // 1st block never free
    // blocks len < 10
    int *free_space_hints[10];

    for (int i = 0; i < 10; i++)
        free_space_hints[i] = blocks;

    int *end = blocks + n_blocks - 1;

    for (int id = find_last_id(end); id >= 1; id--) {
        int len;
        int *pos = find_id_backwards(id, &end, &len);
        if (pos < free_space_hints[0])
            break;

        int *target = find_free_block(free_space_hints[len], pos, len);

        free_space_hints[len] = target;

        if (target < pos) {
            move_block(target, pos, len);
            free_space_hints[len] += len;
        }

        for (int i = len + 1; i < 10; i++)
            if (free_space_hints[i] < free_space_hints[len])
                free_space_hints[i] = free_space_hints[len];

#if 0
        for (int i = 0; i < n_blocks; i++) {
            if (blocks[i] < 0)
                printf(".");
            else
                printf("%d", blocks[i]);
        }
        printf("\n");
#endif
    }

    return blocks;
}

uint64_t calc_checksum(int *disk, int size)
{
    uint64_t checksum = 0;

    for (int i = 0; i < size; i++)
        if (disk[i] != FREESPACE)
            checksum += disk[i] * i;

    return checksum;
}

int main()
{

    size_t filesize;
    char *input = xload_file("input", &filesize);

    int disk_size;
    int *disk_layout = compact_to_id(input, &disk_size);

    //Problem 1
    disk_layout = defragment_simple(disk_layout, disk_size);
#if 0
    for (int i = 0; i < disk_size; i++) {
        if (disk_layout[i] < 0)
            printf(".");
        else
            printf("|%d", disk_layout[i]);
    }
    printf("\n");
#endif

    printf("Problem 1: %lu\n", calc_checksum(disk_layout, disk_size));

    //Probably 2
    xfree(disk_layout);
    disk_layout = compact_to_id(input, &disk_size);

    disk_layout = defragment(disk_layout, disk_size);

    printf("Problem 2: %lu\n", calc_checksum(disk_layout, disk_size));

    xfree(disk_layout);
    xfree(input);

    return EXIT_SUCCESS;
}
