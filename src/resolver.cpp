#include "includes/resolver.h"

Resolver::Resolver(Board* board, KingPiece* _white_king, KingPiece* _black_king, vector<ChessPiece>* _test_pieces_white, vector<ChessPiece>* _test_pieces_black) {
    chessboard = board;
    white_king = _white_king;
    black_king = _black_king;
    test_pieces_white = _test_pieces_white;
    test_pieces_black = _test_pieces_black;
}

/**
 * Finds if the given King is in check.
 */
bool Resolver::IsInCheck(KingPiece* king, bool is_test) {
    vector<ChessPiece*> threats;
    GetThreatened(threats, king->GetLocation(), king->player, true, is_test);
    return threats.size() > 0;
}

/**
 * Converts the move set of a given piece to a vector of tiles that the piece can physically move to.
 * Does not check if the move will put the King in check, only that the tiles are unobstructed or a capture can be made.
 * TODO add en_passant and castling checks
 */
void Resolver::ResolveMoves(ChessPiece& piece, vector<Tile*>& resolved_moves) {
    // piece location
    sCoords location = piece.GetLocation();
    if (piece.captures_with_moves) {
        // moves and captures are the same so check them at all at once
        for (int i=0; i<piece.moves.size(); i++) {
            sRelCoords move = piece.moves[i];
            ResolveSingleRelMove(move, location, resolved_moves, true, piece.player);
        }

    }
    else {
        // check moves
        for (int i=0; i<piece.moves.size(); i++) {
            sRelCoords move = piece.moves[i];
            // check initial moves
            if (piece.is_unmoved && piece.initial_move_repeat_count > 0) {
                ResolveSingleRelMove(move, location, resolved_moves, false, piece.player,
                        piece.initial_move_repeat_count, true);
            }
            // check regular moves
            else {
                ResolveSingleRelMove(move, location, resolved_moves, false, piece.player);
            }
        }
        // check captures
        for (int i=0; i<piece.captures.size(); i++) {
            sRelCoords move = piece.captures[i];
            ResolveSingleRelMove(move, location, resolved_moves, false, piece.player);
        }
    }
}

/**
 * Resolves a single movement direction represented by an sRelCoords object. Updates resolved_moves in place.
 */
void Resolver::ResolveSingleRelMove(sRelCoords& move, sCoords& location, vector<Tile*>& resolved_moves, bool can_capture, ePlayer player, bool is_test, int inital_move_repeat_count, bool test_repeat_init_move) {
    Tile* tile;
    if (!move.repeat && !(test_repeat_init_move && inital_move_repeat_count > 0)) {
        // check move is available
        tile = chessboard->GetTile(location + move);
        ResolveThisMove(tile, resolved_moves, can_capture, player, is_test);
    }
    else {
        int x=1;
        int t=inital_move_repeat_count;
        do {
            if (test_repeat_init_move && t < 0) {
                break; // stop repeat testing once inital_move_repeat_count is used up
            }
            // check all repeat moves in the direction given by moves
            tile = chessboard->GetTile(location + (move * x));
            t--;
            x++;
        } while (!ResolveThisMove(tile, resolved_moves, can_capture, player, is_test));
    }
}

/**
 * Resolves the fetched tile, returns true if the tile was the end of a repeat pattern,
 * and false if a repeat move may still exist.
 * TODO add en_passant and castling checks
 */
bool Resolver::ResolveThisMove(Tile* tile, vector<Tile*>& resolved_moves, bool can_capture, ePlayer player, bool is_test){
    // check the tile is on the board
    if (tile != nullptr) {
        // check if the tile is empty
        if (tile->GetPiece(is_test) == nullptr) {
            // add tile to resolved moves
            resolved_moves.push_back(tile);
            return false;
        }
        // check if the occupying piece is an enemy piece
        else if (can_capture && tile->GetPiece(is_test)->player != player){
            // add tile and end
            resolved_moves.push_back(tile);
            return true;
        }
        // tile was either not occupied by a enemy piece, or could not move to tile.
        else {
            return true;
        }
    }
    // tile was not on the board
    else {
        return true;
    }
}

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
void Resolver::GetThreatened(vector<ChessPiece*>& threats, sCoords location, ePlayer player, bool return_immediately, bool is_test) {
    // get the test pieces. The move sets could be different, so get the other player's pieces
    vector<ChessPiece>* test_pieces = player == BlackPlayer ? test_pieces_white : test_pieces_black;
    // set the position of the test pieces to the test position
    // this will set all the other test pieces since they all point to the same location
    (*test_pieces)[0].SetLocation(location);
    // check each piece in the test piece vector
    for (int i=0; i<test_pieces->size(); i++) {
        // get a piece
        ChessPiece* piece = &(*test_pieces)[i];
        // setup resolver vector for this piece
        vector<Tile*> resolved_moves;
        // resolve each capture move
        for (int j=0; j<piece->captures.size(); j++) {
            // check each capture move one at a time and invert their direction
            sRelCoords move = piece->captures[j].invert();
            // resolve moves with other player's inverted move set but with this player's color.
            // will find all of the other player's pieces that can capture this player's color at the test location.
            ResolveSingleRelMove(move, location, resolved_moves, true, player, is_test);
            // clear out results that are empty
            for (int x=0; x<resolved_moves.size(); x++) {
                ChessPiece* test_threat = resolved_moves[x]->GetPiece(is_test);
                // check the tile is not empty
                if (test_threat != nullptr) {
                    // piece on tile, check that its type matches the test piece.
                    // this is to prevent adding a threat more than once
                    if (test_threat->type == piece->type) {
                        // same type add to threats
                        threats.push_back(test_threat);
                        // break if return_immediately is enabled
                        if (return_immediately) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

/**
 * Filters the resolved_moves vector to only contain moves that do not put the king in check.
 */
void Resolver::ValidateResolvedMoves(vector<Tile*>& resolved_moves, ChessPiece* moving_piece) {
    vector<Tile*> valid_moves;
    // validate every move in resolved_moves
    for (int i=0; i<resolved_moves.size(); i++) {
        bool valid = ValidateMove(resolved_moves[i]->GetLocation(), moving_piece);
        if (valid) {
            valid_moves.push_back(resolved_moves[i]);
        }
    }
    // update resolved_moves with only the valid moves
    resolved_moves = valid_moves;
}

/**
 * Checks that the given move will not result in the King for the moving player being placed in check.
 */
bool Resolver::ValidateMove(sCoords move, ChessPiece* moving_piece) {
    // test move piece to location
    // get the destination tile
    Tile* move_tile = chessboard->GetTile(move);
    // get the home tile
    Tile* home_tile = chessboard->GetTile(moving_piece->GetLocation());
    // set destination tile to have the test piece
    move_tile->TestPieceSet(moving_piece);
    // set the home tile to not have the testing piece
    home_tile->TestPieceStash();

    // get king for this player
    KingPiece* king = moving_piece->player == WhitePlayer ? white_king : black_king;
    // see if king is in check now
    bool bad_move = IsInCheck(king, true);

    // clear test moves
    move_tile->TestPieceRemove();
    home_tile->TestPieceRestore();
    return !bad_move;
}
