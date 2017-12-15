# vertical-puzzle
Project of Introduction to Artificial Intelligence

## Introduction

The program is meant to solve vertical puzzles like

```
    ABCD
+   ABED
________
   EDCAD
```

and

```
   ABC
*   DC
______
  DEAC
  7ED
______
  FDBC
```

by DFS with efficient pruning. The following operations are supported:

-   Addition, subtraction and multiplication
-   Mixed addition and subtraction of multiple numbers
-   Concatenated vertical puzzles, i.e. the output of the previous puzzle directly used of the input of the next puzzle.

The letters are assumed to stand for mutually distinct digits, so only `A` to `J` are supported.

The program has a CLI and a GUI version. The GUI version is designed to support puzzles that would fit in a 15x15 matrix, while the CLI version can support arbitrarily large puzzles (as long as they would not overflow the data types in C++ [lol]).

## Instructions for using CLI

-   Compile the CLI project (C++11 required; tested under Apple LLVM 8.1.0 `clang++` under macOS 10.12.6):

  ```bash
  cd cli
  cmake .
  make
  ```

-   Compose puzzle files (use `_` for horizontal lines; see `samples` for examples).

-   Run the CLI program:


    ```bash
    ./vertical-puzzle-cli <path-to-puzzle> <max-solutions> [-v]
    ```

    -   `path-to-puzzle`: path to the puzzle file
    -   `max-solutions`: the maximum number of solutions to search for before stopping
    -   `-v` (optional): verbose mode (i.e. print intermediate results)

## Instructions for using GUI

-   Compile the GUI project (`gui/gui.pro`) using `qmake` or Qt Creator (C++11 and Qt5 required; tested under Apple LLVM 8.10 `clang++` and Qt 5.9.1 under macOS 10.12.6).
-   Start the app, and compose a puzzle by:
    -   Double-click on a cell in the input area and input characters by keyboard (use `_` for horizontal line)
    -   Drag a colored tile from the right side and drop it in the input area (when `_` is dropped, the entire line wil be filled)
    -   Choose "File > Load Puzzle" from menu bar
    -   Drag a puzzle file into the window and drop it in the input area
-   Click "Solve" to view the solutions; "Back" to switch back to the puzzle.