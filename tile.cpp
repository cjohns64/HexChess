// #include "./util.cpp"
#include "./chesspiece.cpp"

// Contains information and functions associated with a single tile.
class Tile {
    private:
        ChessPiece* current_piece;

    public:
        eColor tile_color;
        
        Tile(eColor tile_color) {
            tile_color = tile_color;
        }

        /**
         * Places the given piece on this tile, updating current_piece with the reference to the piece.
         */
        void SetPiece(ChessPiece* piece) {
            current_piece = piece;
        }

        /**
         * Retrieves the piece reference located on the tile, if any.
         */
        ChessPiece* GetPiece() {
            return current_piece;
        }
};
