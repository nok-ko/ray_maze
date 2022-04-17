// Serialization/exporting code for converting mazes to "MAZ"-format files.
// Created by Nokko on 2022-04-16.
//

#ifndef RAY_MAZE_EXPORT_H
#define RAY_MAZE_EXPORT_H

#include <stdint.h>
#include "maze.h"
#include "mazformat.h"


// Raw data dumps without width+height info.
struct maze_dump *dump_unpacked(struct maze *m);

struct maze_dump *dump_packed(struct maze *m);

// Write a `.rawmaz` dump to disk.
// Essentially useless, here for debug reasons.
// Just use PM_Unpacked if you want this.
void write_dump(struct maze_dump *d, const char *filename);

// Write a `.maz` file to disk, with the specified filename and packing method.
void write_maz(struct maze *m, uint8_t packing_method, const char *filename);

#endif //RAY_MAZE_EXPORT_H
