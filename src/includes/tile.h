#ifndef TILE_H
#define TILE_H
#include "./util.h"
#include "./chesspiece.h"
// Contains information and functions associated with a single tile.
class Tile {
    private:
        ChessPiece* current_piece;
        ChessPiece* en_passant_piece;
        ChessPiece* test_piece; // place to store a piece when testing moving to a tile
        ChessPiece* test_stashed; // place to stash the current piece to test moving it from this tile
        sCoords position;

    public:
        eColor tile_color;
        Tile(eColor tile_color, sCoords location);
        sCoords GetLocation();

        /**
         * Places the given piece on this tile, updating current_piece with the reference to the piece.
         */
        void SetPiece(ChessPiece* piece, bool en_passant=false);

        /**
         * Retrieves the piece reference located on the tile, if any.
         */
        ChessPiece* GetPiece(bool testing=false, bool en_passant=false);

        /**
         * Clear the piece on tile, use SetPiece to replace the piece if possible.
         */
        void RemovePiece();
        void TestPieceSet(ChessPiece* piece);
        void TestPieceRemove();
        void TestPieceStash();
        void TestPieceRestore();
};
#endif
