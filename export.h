//
// Created by Nokko on 2022-04-16.
//

#ifndef RAY_MAZE_EXPORT_H
#define RAY_MAZE_EXPORT_H

#include <stdint.h>
#include "maze.h"


// More or less raw data, use a struct packed_chunk at higher levels.
struct maze_dump {
    int width;
    int height;
    size_t size;
    unsigned char *dump;
};

struct packed_chunk {
    uint32_t width;
    uint32_t height;
    size_t len;
    uint8_t packing_method;
    uint8_t *data;
};

// .MAZ Format File Header
static const char MAZ_HEADER[8] = "\xe4\xe5maze<3";

// Packing Method constants.
// Need to be static to prevent linkage issues.
static const uint8_t PM_Packed = 0;
static const uint8_t PM_Unpacked = 1;
static const uint8_t PM_RESERVED = 63;

// Raw data dumps without width+height info.
struct maze_dump *dump_unpacked(struct maze *m);

struct maze_dump *dump_packed(struct maze *m);

// Write a `.rawmaz` dump to disk.
void write_dump(struct maze_dump *d, const char *filename);

// Write a `.maz` file to disk, with the specified filename and packing method.
void write_maz(struct maze *m, uint8_t packing_method, const char *filename);

#endif //RAY_MAZE_EXPORT_H
