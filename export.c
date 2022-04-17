//
// Simple and not-simple maze-data serialization functions.
// Created by Nokko on 2022-04-16.
//

#include <stdio.h>
#include "export.h"


// Unpacked dump, mostly for debug purposes.
struct maze_dump *dump_unpacked(struct maze *m) {
    struct maze_dump *md = malloc(sizeof(struct maze_dump));
    
    // Unpacked, so 1 char per thing.
    const size_t size = m->width * m->height;
    md->dump = malloc(size * sizeof(unsigned char));
    unsigned char *dump = md->dump;

    // Failed to alloc memory…
    if (md == NULL || dump == NULL) {
        free(dump);
        free(md);
        return NULL;
    }

    md->width = m->width;
    md->height = m->height;
    md->size = size;

    unsigned char *dmp = dump;
    for (int *d = m->data; d < (m->data + size); d++, dmp++) {
        // Lowest bits, hopefully
        *dmp = '\0' | *d;
    }
    return md;
}

// Packed dump, 1 nibble/maze cell.
struct maze_dump *dump_packed(struct maze *m) {
    struct maze_dump *md = malloc(sizeof(struct maze_dump));

    // The actual size of the data we're working with:
    const size_t real_size = (m->width * m->height);

    // Packed, so 0.5 chars per thing. We'll write this many chars.
    // Need to be careful not to read from unowned memory!
    // Ceiling division trick:
    const size_t size = (m->width * m->height + 1) / 2;

    md->dump = malloc(size * sizeof(unsigned char));
    unsigned char *dump = md->dump;

    // Failed to alloc memory…
    if (md == NULL || md->dump == NULL) {
        free(md->dump);
        free(md);
        return NULL;
    }

    md->width = m->width;
    md->height = m->height;
    md->size = size;

    unsigned char *dmp = dump;
    for (int *d = m->data; d < (m->data + real_size); d++, dmp++) {
        // We'll read two bits in one go, and write one.
        // High bits:
        *dmp = '\0' | (*(d++) << 4);
        // Check that we're not over the bounds (again):
        if (d >= m->data + real_size) break;
        // Safe, so write the low bits:
        *dmp |= *d;
    }

    return md;
}

void write_dump(struct maze_dump *d, const char *filename) {

}

void write_maz(struct maze *m, uint8_t packing_method, const char *filename) {
    if (m == NULL) return;

    struct maze_dump *md = NULL;
    if (packing_method > PM_Unpacked) return;

    switch (packing_method) {
        case PM_Unpacked:
            md = dump_unpacked(m);
            break;
        case PM_Packed:
            md = dump_packed(m);
            break;
        default:
            return;
    }

    if (md == NULL) return;

    struct packed_chunk chunk = {
           md->width,
           md->height,
           md->size,
           packing_method,
           md->dump
    };

    FILE *f = fopen(filename, "w");

    // Write header bytes
    fwrite(MAZ_HEADER, sizeof(MAZ_HEADER[0]), 8, f);

    // Write 16-byte padding
    const uint8_t padding[16] = { 0 };
    fwrite(padding, sizeof(uint8_t), 16, f);

    // Write width and height fields
    const uint32_t width_height[2] = {md->width, md->height};
    fwrite(width_height, sizeof(uint32_t), 2, f);

    // Write packing method
    fwrite(&packing_method, sizeof(uint8_t), 1, f);

    // Write maze data
    fwrite(md->dump, sizeof(unsigned char), md->size, f);

    // Finish up.
    fclose(f);
}
