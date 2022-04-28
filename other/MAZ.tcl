# Binary Template for the MAZ file format.
# See:

requires 0 "e4 e5 6d 61 7a 65 3c 33"; # Header

section "MazHeader" {
  bytes 8 "HeaderConstant"
  bytes 16 "HeaderPadding"
}

set width [uint32]
set height [uint32]

section "Dimensions" {
  entry "Width" $width 4 [expr [pos] - 8]
  entry "Height" $height 4 [expr [pos] - 4]

  set maze_size [expr ($width * $height)]

  set data_len 0
  set packing_method [uint8]
  switch $packing_method {
    0 {
      set data_len [expr ceil($maze_size / 2) + $maze_size % 2]
      entry "Size (Packed)" [expr int($data_len)]
      set packing_method "Simple Raw Packing"
    }
    1 {
      set data_len $maze_size
      set packing_method "Raw Maze Dump"
    }
  }
  entry "Size (Actual)" $maze_size
}

entry "Packing Method" $packing_method 1 [expr [pos] - 1]

section "Maze Data" {
  bytes [expr int($data_len)] "Data"
}
