#ifndef RESOLVER_H
#define RESOLVER_H
#include "../includes/board.h"
#include "../includes/util.h"

class Resolver {
private:
    Board* chessboard;
    KingPiece* white_king;
    KingPiece* black_king;
    vector<ChessPiece>* test_pieces_white;
    vector<ChessPiece>* test_pieces_black;

public:
    Resolver(Board* board, KingPiece* _white_king, KingPiece* _black_king, vector<ChessPiece>* _test_pieces_white, vector<ChessPiece>* _test_pieces_black);

    /**
     * Finds if the given King is in check.
     */
    bool IsInCheck(KingPiece* king, bool is_test=false);

    /**
     * Converts the move set of a given piece to a vector of tiles that the piece can physically move to.
     * Does not check if the move will put the King in check, only that the tiles are unobstructed or a capture can be made.
     */
    void ResolveMoves(ChessPiece& piece, vector<Tile*>& resolved_moves);

    /**
     * Resolves a single movement direction represented by an sRelCoords object. Updates resolved_moves in place.
     */
    void ResolveSingleRelMove(sRelCoords& move, sCoords& location, vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer, bool is_test=false, int inital_move_repeat_count=0, bool test_repeat_init_move=false);

    /**
     * Resolves the fetched tile, returns true if the tile was the end of a repeat pattern,
     * and false if a repeat move may still exist.
     */
    bool ResolveThisMove(Tile* tile, vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer, bool is_test=false);

    /**
     * Function given a tile, will lookup pieces of the other player that threaten the piece on the input tile.
     * If return_immediately is true, it will return after the first threat is found.
     *
     * input:
     * location: location of tile that will be checked if it is under threat by the other player.
     * player: the player whose piece on the input tile may be threatened. Can be found by checking the piece on the tile.
     * (optional) return immediately: if True, will return the first threatening piece found instead of searching for all of them.
     * 
     * output:
     * list of pieces that threaten the piece on the given tile
     * 
     * The check is done by looking checking each possible move (inverted direction)
     * with each piece type using the other player's move set.
     */
    void GetThreatened(vector<ChessPiece*>& threats, sCoords location, ePlayer player, bool return_immediately=false, bool is_test=false);

    /**
     * Filters the resolved_moves vector to only contain moves that do not put the king in check.
     */
    void ValidateResolvedMoves(vector<Tile*>& resolved_moves, ChessPiece* moving_piece);

    /**
     * Checks that the given move will not result in the King for the moving player being placed in check.
     */
    bool ValidateMove(sCoords move, ChessPiece* moving_piece);
};
#endif
