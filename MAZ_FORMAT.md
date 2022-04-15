
# The MAZ File Format

This Git repository contains several pieces of code that read and write to `MAZ` files, associated with the `.maz` file extension. This document aims to give motivation for the `MAZ` file format, describe its structure, and provide a starting point to implementers of `MAZ` serialization and deserialization code.

## Background

Maze generation is a favourite pastime of many programmers. [[1]][1] The author of this document, one <span style="color: rgba(1,1,1,0.0); background: linear-gradient(to right, orange, magenta); -webkit-background-clip: text;">nokko</span>, has cultivated a particular fascination with maze-generating, -analyzing, and -solving programs. Though many styles of mazes can be generated algorithmically, let us consider two-dimensional, “rectangular” mazes.

A rectangular maze can be drawn on graph paper. It has a width and a height, call them `w` and `h`. There are `w * h` cells in a rectangular maze. Each cell has 4 bits of data associated with it: the `N`, `E`, `S`, and `W` bits.  The bits correspond to the cardinal directions: North, East, South, and West.
<sup><a href="#footnote_cardinal" ref="ref_cardinal">[2‍]</a></sup>

Each bit with a value of `1` represents a path between a cell and its neighbour. Accordingly, a bit with value `0` represents a wall. In this document, when referring to a cell with value `0b0110`, we mean a cell with the `E` and `S` bits set to `1`, and the `N` and `W` bits set to `0`. The order of the bits, `NESW` is arbitrary.

<section class="footnotes" style="font-size:smaller; background-color: rgba(0,0,0,0.2); padding-top: 0.1rem; padding-bottom: 0.1rem;">
<p id="footnote_cardinal" style="margin-left: 1rem">
    <a href="#ref_cardinal">[2‍]</a>
    Or the <code>U</code>, <code>R</code>,<code>D</code>, and <code>L</code> bits, if you aren't cartographically inclined.
</p>
<hr/>
<p style="margin-left: 1rem">
Equivalently, a more mathematically-inclined reader may consider a graph where each vertex has at least 2 and at most 4 connections as the “stone” into which a maze is carved – a <a href="https://en.wikipedia.org/wiki/Lattice_graph#Square_grid_graph">square grid graph</a>.
</p>
<p style="margin-left: 1rem">
So, a maze carved into the stone is just a subset of the graph's edges. I mean, most subsets are probably pretty bad mazes, but you can call them mazes, and, more importantly, represent them in <code>MAZ</code> format.
</p>
<p style="margin-left: 1rem">
Then, a “perfect” maze is a <a href="https://en.wikipedia.org/wiki/Spanning_tree">spanning tree</a> of such a graph, where there is a path from each vertex to each other vertex. All maze-generating algorithms that produce perfect mazes, really produce spanning trees. Ain’t that neat.
</p>
</section>

Figure 1 contains a small maze rendered in monospace ASCII characters, where each cell is walled off.

<figure>

```
#########
# ## ## #
#########
#########
# ## ## #
#########
#########
# ## ## #
#########
```
<figcaption>
A maze with <code>w=3, h=3</code>. All cell values are <code>0b0000</code>. Each cell is displayed with 9 characters – A more compact ASCII display is possible, but will not reproduce all of the detail in an imperfect maze, because it assumes the neighbours' bits are consistent with each other. 
</figcaption>

</figure>

## A Simple Approach

A generally good strategy for storing maze data *as it is being generated* <sup><a href="footnode_generated" id="r2">[2‍]</a></sup> (though not the only viable strategy, especially for [certain algorithms][3] with reduced memory requirements,) is to use a simple two-dimensional array of bytes.


<section class="footnotes" style="font-size:smaller; background-color: rgba(0,0,0,0.2); padding-top: 0.1rem; padding-bottom: 0.1rem;">
<p id="footnote_generated" style="margin-left: 1rem">
    <a href="#r2">[1‍]</a>
    Note that we're not talking about storing data for other purposes, yet.
</p>
</section>

Then, to carve a path between a cell at array index `current` in the two-dimensional array `m`, and its southern neighbour:
* Set the `S` bit on `m[current]`
* Set the `N` bit on `m[w + current]`

For programmer convenience, the direction bits `NESW` are often put into integer constants:

```c
const char N = 8 // 0b1000
const char E = 4 // 0b0100
const char S = 2 // 0b0010
const char W = 1 // 0b0001
```

And are composed with bitwise operators:

```c
void carve_south(char *cell, struct maze m) {
    *cell |= S;
    *(cell + m.w) |= N;
}
```

<section class="footnotes" style="font-size:smaller; background-color: rgba(0,0,0,0.2); padding-top: 0.1rem; padding-bottom: 0.1rem;">
<p style="margin-left: 1rem">
    The above code is a C implementation of the “carve south” procedure described in pseudocode a couple of paragraphs ago. It uses pointers, but you could write a version that works with indices instead.
</p>
</section>

Likewise, the bitwise AND operator can be used to check whether a given cell has an opening in a given direction:

```c
bool goes_south(char *cell) {
    return (*cell & S) != 0;
}
```

Note that earlier we said that we would store the maze in an array of bytes as it was being generated, and indeed, the above code uses the `char` datatype (8 bits on most machines) to refer to the values of cells in the maze. But we only make use of the lower 4 bits of this `char`, because we'll never write a value greater than `0b00001111` into the maze!

## Motivation

In short: I wanted to create my own bitmap file format, specifically for storing maze data.

Like many bitmap formats, it should have a few header bytes that clarify what dimensions the maze should take. At its most basic, it's more or less equivalent to a bit-depth 4 uncompressed bitmap.

It also supports including auxilliary data, for example: Comments, annotations with associated bounding boxes, information about the generation method and the author's name, as well as labelled blobs of data containing information about each cell. (Colours, IDs, etc.)

<section class="footnotes" style="font-size:smaller; background-color: rgba(0,0,0,0.2); padding-top: 0.1rem; padding-bottom: 0.1rem;">
<figure style="margin-left: 1rem;">
<pre>
####################################
#---------------------------------+#
##################################|#
##################################|#
#+--------------------------------+#
#|##################################
#|##################################
#+--------------------------------o#
####################################

TO jshape
fd 12
rt 90
fd 1
rt 90
END
TO lshape
fd 12
lt 90
fd 1
lt 90
END
rt 90
jshape
lshape
jshape
lshape
</pre>
</figure>
<p style="margin-left: 1rem">
Maze data can also be represented as a sort of LOGO-style program. (See above.)
</p>
<p style="margin-left: 1rem">
I suspect that for some mazes, naïvely compressing the bitmap representation will be a lot less effective than compressing the procedural-program-style version. (Ones with lots of regular squiggles and straight lines?) So, another motivation is to test this theory, and if it bears fruit, support more compressable mazes!
</p>
</section>


As already discussed, while generating mazes, it's convenient to not care too much about memory usage – it's okay to use a whole byte when a nibble would do. But if we want to store the generated maze for later display/analysis/critique/etc., it seems that emitting a whole byte for each cell is very wasteful – a 50x50 maze *could* be represented in 1,250 bytes, but just dumping it out of memory would produce 2,500.

At time of writing, (this document is being written concurrently with the implementation of the first version of serialization and deserialization code for the `MAZ` format,) the author has written 5 implementations of one [“Recursive Backtracker”][1] algorithm for maze generation, across 3 different languages.

I also expect to write more maze-related utilities, for example structure analyzers and solvers, that don't generate their own mazes but must instead read them from an input file – so I need a file *format*, so why not make my own.

## MAZ Version 1 Spec

### File Header

All `MAZ`-format files start with some header bytes:

```hex
e4 e5 6d 61 7a 65 3c 33
```

<section class="footnotes" style="font-size:smaller; background-color: rgba(0,0,0,0.2); padding-top: 0.1rem; padding-bottom: 0.1rem;">
<p style="margin-left: 1rem">
    This spells <code>"\xe4\xe5\maze<3"</code>.
</p>
</section>


The header contains intentionally-blank padding of 16 bytes:

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
```

This is meant to be filled with more useful stuff further down the line – version and extension information.

The next 8 bytes are the `width` and `height` fields – 32-bit unsigned integers.

```
00 00 00 05 00 00 00 05
```

### Packed Data

The next chunk contains the actual maze data. It begins with a single byte defining the packing method – at present the only defined method is Simple Raw Packing. In the future the LOGO-style method defined above, and compression techniques like Run Length Encoding may be supported. A value of `ff` indicates that this isn't a packed data chunk - this is a reserved value for extensions to the format.

```
+======+====================+
| Byte |   Packing Method   |
+======+====================+
|  00  | Simple Raw Packing |
+------+--------------------+
|  ff  |     (Reserved)     |
+------+--------------------+
```

The following `ceil((width * height) / 2)` bytes are the packed representation of the maze data. When reconstituting the packed data into a byte-array, a deserializer would consume a byte, write the high nibble to the array, move to the next location in the array, then, write the low nibble into the array.

In a maze with an odd number of cells, the last nibble in the packed data section should be set to `0000`.

## Reference Implementation

See `maz_export.c` and `maz_import.c` in this repository. (TBD)


[1]: https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking

[3]: https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm