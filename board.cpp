#include "./tile.cpp"
#include <vector>


class Board {
    private:
        // data structure representing the board
        // tile sequence is White, Black, Grey
        std::vector<std::vector<Tile>> chessboard = {
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB)},
            {Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB)},
            {Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG)},
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), },
            {Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB)},
            {Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG)},
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG)},
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB)},
            {Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG)},
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB), Tile(colorG)},
            {Tile(colorW), Tile(colorB), Tile(colorG), Tile(colorW), Tile(colorB)}
            };

        /**
         * Checks if the given location is on the board
         */
        bool IsOnBoard(sCoords location) {
            // check rank is within range
            if (location.rank < 10 and location.rank >= 0) {
                // check file is within range
                switch (location.rank) {
                    case 0:
                        return location.file <= 5 and location.file >= 0;
                    case 1:
                        return location.file <= 6 and location.file >= 0;
                    case 2:
                        return location.file <= 7 and location.file >= 0;
                    case 3:
                        return location.file <= 8 and location.file >= 0;
                    case 4:
                        return location.file <= 9 and location.file >= 0;
                    case 5:
                        return location.file <= 10 and location.file >= 0;
                    case 6:
                        return location.file <= 9 and location.file >= 0;
                    case 7:
                        return location.file <= 8 and location.file >= 0;
                    case 8:
                        return location.file <= 7 and location.file >= 0;
                    case 9:
                        return location.file <= 6 and location.file >= 0;
                    case 10:
                        return location.file <= 5 and location.file >= 0;
                    default:
                        return false;
                }
            }
            else {
                return false;
            }
        }

    public:
        Board() {
            // int x=0;
            // int k=0;
            // for (int i=0; i<MAX_BOARD_LENGTH; i++) {
            //     for (int j=0; j<5+x; j++) {
            //         // grow by 1 for first half, then shrink by one for second half
            //         if (i >= 5) {
            //             x--;
            //         }
            //         else {
            //             x++;
            //         }
            //         // tile pattern is White, Black, Grey
            //         switch (k%3){
            //             case 0:
            //                 chessboard[i][j] = Tile(colorW);
            //                 k++;
            //                 break;
            //             case 1:
            //                 chessboard[i][j] = Tile(colorB);
            //                 k++;
            //                 break;
            //             case 2:
            //                 chessboard[i][j] = Tile(colorG);
            //                 k++;
            //                 break;
            //         }
            //     }
            // }
        }

        /**
         * Retrieves a reference to the tile at the given coordinates. Checks if the tile is on the board first.
         * Returns a pointer to the tile or a null pointer if the tile is not on the board.
         */
        Tile* GetTile(sCoords location) {
            if (IsOnBoard(location)) {
                return &chessboard[location.rank][location.file];
            }
            else {
                return nullptr;
            }
        }

};
