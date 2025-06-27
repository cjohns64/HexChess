## Project Design
### Board:
Instead of a regular square tiled chess board this game uses a hexagonal tiled board. This affects the game logic and initial setup slightly. Design choices were made with the goal of preserving most of the game-play of the original. The most notable differences being:

1. There are 3 tile colors, this requires 3 bishops.
2. Diagonals jump a tile, since hexagons share a side with all adjacent hexagons (unlike squares which share a vertex, but not a side, with 4 of their neighbors). Here I am defining diagonals as tiles that are not along a rank or file. For both square and hexagonal boards, diagonals have the same tile color.
3. Hexagonal tiles actually have 3 “ranks” while square tiles have 2 (named ranks for rows and files for columns).
4. Due to the increased freedom of movement, the hexagonal board had to be scaled up significantly to preserve the feel of the original game, 91 tiles instead of 64.
5. Due to the diagonal skipping, and the difficulty of building a pawn chain, the number of pawns was doubled.

![Initial setup, note: 3 bishops, 3 knights, and 16 pawns](images/figure-01.jpg)

*Proposed initial setup; note that there are 3 bishops, 3 knights, and 16 pawns for each player.*
### Pieces:

- King
  - Captures by movement
  - Moves one tile in any direction
  - Cannot move into check
  - Cannot castle if in check or passes through a tile under threat
- Queen
  - Captures by movement
  - Move any number of squares along a rank, file, or diagonal
- Rook
  - Captures by movement
  - Move any number of squares along a rank or file
  - Can be castled with King if both pieces have not moved, there are no obstructions, and the King does not pass through a threatened space.
- Knight
  - Moves to any of the closest squares that are not on the same rank, file, or diagonal
  - Captures by movement
- Bishop
  - Diagonal Movement
  - Captures by movement
- Pawn
  - Moves forward 1 space
  - Captures diagonally
  - Moves 1-2 on first movement
  - Other pawns can capture a pawn that moved 2 tiles on the previous round as if it moved 1 tile that round.
  - On advancement to last rank, the pawn is promoted to a Queen, Rook, Bishop, or Knight.

![Move sets for each piece, overlayed on-top of their initial positions](images/figure-02.jpg)

*Some moves in the figure above are blocked by other pieces, but were included to illustrate the selected piece’s full moveset.*

## Project Implementation
### C++ Backend
The project was first implemented as a C++ program. This program runs in the terminal and prints the state of the board in a simple text format. I chose C++ because I wanted to learn it and I had no prior experience with it. This command line C++ program can be run independently from the Godot frontend game, but it is not interactive.

### Godot Frontend
In order to improve the user interface, and to give me an opportunity to learn the engine. I used the Godot game engine to build a 3D interface for the C++ code. This required using GDExtension to connect the C++ backend to the GDScript frontend. The GDScript code manages the user input and displaying the game state, but does not manage any of the game state.

### Development Process
In the course of developing this game, I found the two versions of the game to be helpful for debugging. I found an effective strategy of running the Godot game until a bug was found, then copying the log of moves to the command line code and debugging it using gdb.

### Assets
I made all the assets for this project from scratch. The 3D models were made in Blender. None of the models required animation and texturing was done with a simple procedural material that I baked into a texture map. I recorded the sound myself and processed it with Tenacity.

## Final Build
![Final game interface, available moves for the selected piece are highlighted in blue](images/figure-03.jpg)

*Pieces can be selected and show the available moves they can make, an undo selection button is visible only once a piece is selected.*

![A game over screen, the game can detect 5 distinct game over conditions](images/figure-04.jpg)

*The game can detect 5 different game over conditions: Checkmate, Stalemate, Threefold Repetition, Dead Position, and 50 Move Rule.*
