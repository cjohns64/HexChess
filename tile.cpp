#include "./chesspiece.cpp"

// Contains information and functions associated with a single tile.
class Tile {
    private:
        ChessPiece* current_piece = NULL;
        ChessPiece* test_piece = NULL; // place to store a piece when testing moving to a tile
        ChessPiece* test_stashed = NULL; // place to stash the current piece to test moving it from this tile
        sCoords position;

    public:
        eColor tile_color;
        
        Tile(eColor tile_color, sCoords location): position(location.rank, location.file) {
            tile_color = tile_color;
        }

        sCoords GetLocation() {
            return position;
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
        ChessPiece* GetPiece(bool testing=false) {
            if (testing && test_piece != NULL) {
                // prefer a testing piece if testing mode is on
                return test_piece;
            }
            return current_piece;
        }

        /**
         * Clear the piece on tile, use SetPiece to replace the piece if possible.
         */
        void _RemovePiece_() {
            current_piece = NULL;
        }

        void TestPieceSet(ChessPiece* piece) {
            test_piece = piece;
        }

        void TestPieceRemove() {
            test_piece = NULL;
        }

        void TestPieceStash() {
            test_stashed = current_piece;
        }

        void TestPieceRestore() {
            current_piece = test_stashed;
        }
};
