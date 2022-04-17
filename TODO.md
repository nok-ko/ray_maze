# The TODO File

## Bugs & Issues
* Exporting in unpacked format sometimes writes the wrong number of bytes (half the maze) to disk
* Invalid file operations crash the program
* Address Sanitizer complains about some stuff, still

## Design & Architecture
* The stack data structure in `stack.c` is implemented with a dynamic array - this is pretty bad, for quasi-obvious reasons.
  * A better implementation would use a linked list for its superior O(1) push and pop time complexity.
* `queue.c` is straight-up broken, and unused in the code, anyway.
* The venerable `draw_maze()` function should probably be factored out into a separate file from the `main.c`.
  * Really, it would be a lot nicer if `draw_maze()` took a `Rectangle` bounding box and tried to draw the maze inside that box, instead of asking the caller to provide a `cellWidth` value.
  * Also, if it drew onto a framebuffer instead of to the screen directly, that might have (basically negligible) performance benefits, and more importantly would allow for zooming/panning across a large maze.
* It may behoove me to look into the [GUI Layout Design Tool](https://raylibtech.itch.io/rguilayout) that ray's team put together, and do a redesign of the way the thing looks at the moment.

## Code Quality
* There are lots of unnecessary `typedef`s scattered around the codebase – `maze_t`, `neighbour_t`, `pos_t`, etc.
  * The `_t` prefix is reserved by the POSIX standard and shouldn't be used in the way this project uses it.
  * Really, it makes sense to call a `struct` a `struct`. Non-opaque types shouldn't be `typedef`'d, it just makes things unclear.
* The code uses `style_cyber.h`, pilfered from `raysan5`'s [rfxgen](https://github.com/raysan5/rfxgen) tool. As far as I can tell, I don't have the right license to use this style header, and I ought to make my own.
* 