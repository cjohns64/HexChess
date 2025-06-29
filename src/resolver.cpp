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
bool Resolver::IsInCheck(KingPiece* king) {
    return IsThreatened(king);
}

/**
 * Finds if the given piece is threatened.
 */
bool Resolver::IsThreatened(ChessPiece* piece) {
    vector<ChessPiece*> threats;
    GetThreatened(threats, piece->GetLocation(), piece->player, true);
    return threats.size() > 0;
}

/**
 * Finds if the given tile is threatened by the other player.
 */
bool Resolver::IsThreatened(Tile* tile, ePlayer player) {
    vector<ChessPiece*> threats;
    GetThreatened(threats, tile->GetLocation(), player, true);
    return threats.size() > 0;
}

/**
 * Converts the move set of a given piece to a vector of tiles that the piece can physically move to.
 * Does not check if the move will put the King in check, only that the tiles are unobstructed or a capture can be made.
 * 
 * Move data:
 * - check initial_only
 * - check target_requirement
 * - check tile_requirement
 * - repeat up to repeat_max
 * - stop if conditions are not met or a piece is encountered
 *
 */
void Resolver::ResolveMoves(ChessPiece& piece, vector<Tile*>& resolved_moves) {
    if (!piece.is_alive) {
        return; // dead pieces don't have moves
    }
    // piece location
    sCoords location = piece.GetLocation();
    // check moves
    for (int i=0; i<piece.moves.size(); i++) {
        sRelCoords move = piece.moves[i];
        // skip resolving initial moves if piece has moved
        if ((piece.is_unmoved && move.initial_only) || !move.initial_only) {
            ResolveSingleRelMove(piece.type, move, location, resolved_moves, piece.player);
        }
    }
}

/**
 * Resolves a single movement direction represented by an sRelCoords object. Updates resolved_moves in place.
 * Returns once the move is resolved.
 *
 * Piece Requirement:
 * NoPiece: #tile requirement == TileEmpty
 *  - target is empty
 * AnyPiece: #ignore this req
 * Other: #match
 *  - match piece and tile requirement
 * 
 * Tile Requirement:
 * TileEmpty:
 *  - target empty
 * TileEnemy:
 *  - target player != current player && not empty
 * TileEmptyOrEnemy:
 *  - target player != current player || empty
 * TileAlly:
 *  - target player == current player
 */
void Resolver::ResolveSingleRelMove(eType piece_type, sRelCoords& move, sCoords& location, vector<Tile*>& resolved_moves, ePlayer player) {
    Tile* tile;
    int x=1;
    int t=move.repeat_max;
    // repeat up to repeat_max
    while (t >= 0) { // one loop if t==0
        // check all repeat moves in the direction given by move
        tile = chessboard->GetTile(location + (move * x));
        if (tile != nullptr) {
            // check target_requirement
            if (move.target_requirement == NoPiece) {
                // tile must be empty
                if (tile->GetPiece() != nullptr) {
                    // tile is not empty
                    return;
                }
                else {
                    // add move
                    resolved_moves.push_back(tile);
                }
            }
            else if (move.target_requirement == AnyPiece) {
                ChessPiece* target_piece = tile->GetPiece();
                switch (move.tile_requirement) {
                    case TileEmpty:
                        if (target_piece == nullptr) {
                            resolved_moves.push_back(tile);
                        }
                        else {
                            // condition failed
                            return;
                        }
                        break;
                    case TileEnemy:
                        if (target_piece != nullptr && target_piece->player != player) {
                            resolved_moves.push_back(tile);
                            return; // stop at piece
                        }
                        else if (piece_type == Pawn && target_piece == nullptr) {
                            // check for possible en passant capture
                            if (en_passant_location.rank >= 0 && en_passant_location == tile->GetLocation()) {
                                // en_passant_loc is active and same location as target tile
                                resolved_moves.push_back(tile);
                                return; // stop at piece
                            }
                            else {
                                // condition failed
                                return;
                            }
                        }
                        else {
                            // condition failed
                            return;
                        }
                        break;
                    case TileAlly:
                        if (target_piece != nullptr && target_piece->player == player) {
                            resolved_moves.push_back(tile);
                            return; // stop at piece
                        }
                        else {
                            // condition failed
                            return;
                        }
                        break;
                    case TileEmptyOrEnemy:
                        if (target_piece != nullptr && target_piece->player != player) {
                            resolved_moves.push_back(tile);
                            return; // stop at piece
                        }
                        else if (target_piece == nullptr) {
                            resolved_moves.push_back(tile);
                        }
                        else {
                            // condition failed
                            return;
                        }
                        break;
                    default:
                        // default to failed
                        return;
                }
            }
            else { // target requirement needs piece match
                ChessPiece* target_piece = tile->GetPiece();
                if (target_piece != nullptr) {
                    if (target_piece->type == move.target_requirement) {
                        if (move.tile_requirement == TileEnemy && target_piece->player != player) {
                            // matched tile requirement
                            resolved_moves.push_back(tile);
                            return; // stop at piece
                        }
                        else if (move.tile_requirement == TileAlly && target_piece->player == player) {
                            // matched tile requirement
                            resolved_moves.push_back(tile);
                            return; // stop at piece
                        }
                        else {
                            // no added condition
                            resolved_moves.push_back(tile);
                        }
                    }
                    else {
                        // no match
                        return;
                    }
                }
                else {
                    // no piece so no match
                    return;
                }
            }
            // check tile_requirement
            // stop if conditions are not met or a piece is encountered
        }
        else {
            return; // tile not on board
        }
        t--;
        x++;
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
void Resolver::GetThreatened(vector<ChessPiece*>& threats, sCoords location, ePlayer player, bool return_immediately) {
    // get the test pieces. The move sets could be different, so get the other player's pieces
    vector<ChessPiece>* test_pieces = player == BlackPlayer ? test_pieces_white : test_pieces_black;
    // check each piece in the test piece vector
    for (int i=0; i<test_pieces->size(); i++) {
        // get a piece
        ChessPiece* piece = &(*test_pieces)[i];
        // set the position of the test pieces to the test position
        piece->SetLocation(location);
        // setup resolver vector for this piece
        vector<Tile*> resolved_moves;
        // resolve each capture move
        for (int j=0; j<piece->moves.size(); j++) {
            // check each capture move one at a time and invert their direction
            sRelCoords move = piece->moves[j].invert();
            if (move.target_requirement == NoPiece || move.tile_requirement == TileEmpty) {
                continue; // skip move since it can't threaten a square
            }
            // resolve moves with other player's inverted move set but with this player's color.
            // will find all of the other player's pieces that can capture this player's color at the test location.
            ResolveSingleRelMove(piece->type, move, location, resolved_moves, player);
            // clear out results that are empty
            for (int x=0; x<resolved_moves.size(); x++) {
                ChessPiece* test_threat = resolved_moves[x]->GetPiece();
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
    // ### save start state ###
    sCoords piece_location = moving_piece->GetLocation();
    // get the destination tile
    Tile* move_tile = chessboard->GetTile(move);
    // get the home tile
    Tile* home_tile = chessboard->GetTile(moving_piece->GetLocation());
    ChessPiece* target_piece = move_tile->GetPiece();
    
    // ### set test state ###
    // move piece to test location
    moving_piece->SetLocation(move);
    // set destination tile to have the test piece
    move_tile->SetPiece(moving_piece);
    // set the home tile to not have the testing piece
    home_tile->RemovePiece();

    // ### check test move ###
    // get king for this player
    KingPiece* king = moving_piece->player == WhitePlayer ? white_king : black_king;
    // see if king is in check now
    bool bad_move = IsInCheck(king);

    // ### restore start state
    // clear test moves
    move_tile->SetPiece(target_piece);
    home_tile->SetPiece(moving_piece);
    // reset piece back to its actual location
    moving_piece->SetLocation(piece_location);
    return !bad_move;
}

sCoords Resolver::GetEnPassantLocation() {
    return en_passant_location;
}

void Resolver::SetEnPassantLocation(sCoords location) {
    en_passant_location = location;
}
