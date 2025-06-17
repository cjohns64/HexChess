#include "includes/hex_chess_driver.h"

/**
 * Init:
 * - setup board
 * - place pieces
 * 
 * Run:
 * White Turn:
 * - update round number
 * - update current player = round number % 2
 * - update en-passant | if piece is same player, clear
 * - get valid moves for all white pieces and save/count results in pieces
 * - check for win/draw
 * - select piece
 * - get valid moves from saved set
 * - select? ... undo/redo? ... move?
 * - update game log
 * 
 * Black Turn:
 * - same as White
 */
HexChessDriver::HexChessDriver() : white_king(KingPiece(WhitePlayer, inital_placement[0])),
           black_king(KingPiece(BlackPlayer, inital_placement[26])),
           resolver(Resolver(&chessboard, &white_king, &black_king, &test_pieces_white, &test_pieces_white))
           {
    // initial placement -- White
    for (int i=1; i<26; i++) {
        if (i>9) {
            white_pieces.push_back(PawnPiece(WhitePlayer, inital_placement[i]));
        }
        else if (i>1 && i<=4) {
            white_pieces.push_back(BishopPiece(WhitePlayer, inital_placement[i]));
        }
        else if (i>4 && i<=7) {
            white_pieces.push_back(KnightPiece(WhitePlayer, inital_placement[i]));
        }
        else if (i>7 && i<=9) {
            white_pieces.push_back(RookPiece(WhitePlayer, inital_placement[i]));
        }
        else if (i==1) {
            white_pieces.push_back(QueenPiece(WhitePlayer, inital_placement[i]));
        }
    }
    // initial placement -- Black 
    for (int i=27; i<52; i++) {
        if (i>35) {
            black_pieces.push_back(PawnPiece(BlackPlayer, inital_placement[i]));
        }
        else if (i>27 && i<=30) {
            black_pieces.push_back(BishopPiece(BlackPlayer, inital_placement[i]));
        }
        else if (i>30 && i<=33) {
            black_pieces.push_back(KnightPiece(BlackPlayer, inital_placement[i]));
        }
        else if (i>33 && i<=35) {
            black_pieces.push_back(RookPiece(BlackPlayer, inital_placement[i]));
        }
        else if (i==27) {
            black_pieces.push_back(QueenPiece(BlackPlayer, inital_placement[i]));
        }
    }
    // set the board with the actual pieces, the board will store pointers to each piece on their current Tile.
    chessboard.SetBoard(white_pieces, white_king, black_pieces, black_king);

    // white promotion tile set
    for (sCoords loc : back_rank_black) {
        white_promotion_tiles.push_back(chessboard.GetTile(loc));
    }
    // black promotion tile set
    for (sCoords loc : back_rank_white) {
        black_promotion_tiles.push_back(chessboard.GetTile(loc));
    }
}

    /**
     * Returns the piece type and player on the given location 
     *
     * Data is encoded as a hexadecimal number stored as an int where:
     *      0 no piece      = 0
     *      1 white king    = 1
     *      2 white queen   = 2
     *      3 white rook    = 3
     *      4 white bishop  = 4
     *      5 white knight  = 5
     *      6 white pawn    = 6
     *      7 black king    = 7
     *      8 black queen   = 8
     *      9 black rook    = 9
     *      a black bishop  = 10
     *      b black knight  = 11
     *      c black pawn    = 12
     *      d - not used    = 13
     *      e - not used    = 14
     *      f - not used    = 15
     */
int HexChessDriver::GetPieceOnTile(int rank, int file) {
    sCoords location = sCoords(rank, static_cast<eFiles>(file));
    Tile* tile = chessboard.GetTile(location);
    if (tile != nullptr && tile->GetPiece() != nullptr) {
        if (tile->GetPiece()->type == NoPiece) {
            return 0;
        }
        else {
            int x = tile->GetPiece()->player == WhitePlayer ? 1 : 7;
            return (int) tile->GetPiece()->type + x;
        }
    }
    else {
        return 0;
    }
}

/**
 * Returns the avalible action type for a given location 
 *
 * Data is encoded as an int where:
 *      0 no action
 *      1 selectable piece
 *      2 move
 *      3 move & selectable piece
 */
int HexChessDriver::GetActionOnTile(int rank, int file) {
    sCoords location = sCoords(rank, static_cast<eFiles>(file));
    bool is_selection = CoordsInCoordsVector(&location, &active_selection);
    bool is_move = CoordsInCoordsVector(&location, &active_moves);

    if (is_selection && is_move) {
        return 3;
    }
    else if (is_move) {
        return 2;
    }
    else if (is_selection) {
        return 1;
    }
    else {
        return 0;
    }
}

int HexChessDriver::GetGameState() {
    return game_state;
}

/**
 * - update round number
 * - update current player = round number % 2
 * - update en-passant | if piece is same player, clear
 * - get valid moves for all current player pieces and save/count results in pieces
 * - check for win/draw
 *
 * Return value is the game over state
 * Should be run before the player is allowed to select a piece
 */
void HexChessDriver::RoundSetup() {
    // update round number
    round_number++;
    // set the current player
    current_player = static_cast<ePlayer>(round_number % 2);
    // skip checking nullptr en passant pieces
    if (en_passant_piece != nullptr) {
        // clear the en_passant_piece if it has made it back to the player that used it
        if (en_passant_piece->player == current_player) {
            en_passant_piece = nullptr;
            // set location to an invalid location
            resolver.SetEnPassantLocation(sCoords(-1, a));
        }
    }
    // check King
    KingPiece *king = current_player == WhitePlayer ? &white_king : &black_king;
    // resolve all moves
    vector<Tile*> resolved_moves;
    resolver.ResolveMoves(*king, resolved_moves);
    // validate all resolved moves
    resolver.ValidateResolvedMoves(resolved_moves, king);
    // clear total valid moves and update with number of valid moves for the king
    total_valid_moves_current_player = resolved_moves.size();
    // update all valid moves for piece
    king->valid_moves_this_turn = TranslateVector(resolved_moves);
    AddCastlingMoves();
    

    // check Pieces
    vector<ChessPiece> *pieces = current_player == WhitePlayer ? &white_pieces : &black_pieces;

    for (int i=0; i<pieces->size(); i++) {
        // resolve all moves
        vector<Tile*> resolved_moves;
        resolver.ResolveMoves((*pieces)[i], resolved_moves);

        // validate all resolved moves
        resolver.ValidateResolvedMoves(resolved_moves, &(*pieces)[i]);
        // add valid moves to total valid moves count
        total_valid_moves_current_player += resolved_moves.size();
        // update all valid moves for piece
        (*pieces)[i].valid_moves_this_turn = TranslateVector(resolved_moves);
    }

    // #### Check for Game End ####
    // check if total number of moves > 0
    if (total_valid_moves_current_player == 0) {
        // STALEMATE OR CHECKMATE
        if (resolver.IsInCheck(king)) {
            game_state = Checkmate;
            return;
        }
        game_state = Stalemate;
        return;
    }
    // check for win/draw conditions
    if (last_capture_round >= 50 || last_pawn_move_round >= 50) {
        // 50 move rule
        game_state = FiftyMoveRule;
        return;
    }
    // check for a dead position
    // not enough material, player does not have any pawns, rooks, or queens
    bool dead_position = true;
    for (int x=0; x<2; x++) {
        vector<ChessPiece> *tmp_pieces = x==0 ? &white_pieces : &black_pieces;
        bool dead_pos[] = {true, true};
        for (int i=0; i<tmp_pieces->size(); i++) {
            if ((*tmp_pieces)[i].type == Pawn && (*tmp_pieces)[i].is_alive) {
                // Not comprehensive
                dead_pos[x] = false;
                break;
            }
            if ((*tmp_pieces)[i].type == Rook && (*tmp_pieces)[i].is_alive) {
                // Not comprehensive
                dead_pos[x] = false;
                break;
            }
            if ((*tmp_pieces)[i].type == Queen && (*tmp_pieces)[i].is_alive) {
                // Not comprehensive
                dead_pos[x] = false;
                break;
            }
        }
        dead_position = dead_pos[0] && dead_pos[1];
    }
    if (dead_position) {
        game_state = DeadPosistion;
        return;
    }

    // 50 move rule setup
    capture_this_round = false;
    pawn_moved = false;
}

/**
 * Checks each promotion tile for a valid promotion, and updates the promotion piece if their is one.
 */
void HexChessDriver::CheckForPromotion() {
    // get the set of tiles where a promotion can happen for this player
    vector<Tile*>* promotion_tiles = current_player == WhitePlayer ? &white_promotion_tiles : &black_promotion_tiles;
    // look through all the tiles for a promotable piece
    for (Tile* tile : *promotion_tiles) {
        ChessPiece* piece = tile->GetPiece();
        if (piece != nullptr && piece->is_alive && piece->is_promotable && piece->player == current_player) {
            // set promotion piece
            promotion_piece = piece;
            // only one piece can be promoted per turn because only one piece can move per turn
            return;
        }
    }
}

/**
 * Returns the location of the promotable piece or 0 if there is none.
 * The location is encoded as a two digit hex number stored as a single decimal number as follows:
 *  rank 0-10 -> 0x1 - 0xb
 *  file a-k  -> 0x1 - 0xb
 *  Return value: rank file -> 0x11 - 0xbb -> 17 - 187
 */
int HexChessDriver::GetPromotionTile() {
    if (promotion_piece != nullptr) {
        int rank = promotion_piece->GetLocation().rank + 1;
        int file = promotion_piece->GetLocation().file + 1;
        return (rank * 16) + file;
    }
    else {
        return 0;
    }
}

/**
 * Promotes the current promotion piece to the given selection.
 * Selection is an int of the following form:
 *  2 = Rook
 *  3 = Bishop
 *  4 = Knight
 *  Any other value = Queen
 */
void HexChessDriver::RunPromotion(int piece_selection) {
    if (promotion_piece == nullptr) {
        return;
    }
    sCoords loc = promotion_piece->GetLocation();
    // update the value pointed to by promotion_piece
    switch (piece_selection) {
        case 2:
            // update promotion piece to a Rook
            *promotion_piece = RookPiece(current_player, loc);
            break;
        case 3:
            // update promotion piece to a Bishop
            *promotion_piece = BishopPiece(current_player, loc);
            break;
        case 4:
            // update promotion piece to a Knight
            *promotion_piece = KnightPiece(current_player, loc);
            break;
        default:
            // update promotion piece to a Queen
            *promotion_piece = QueenPiece(current_player, loc);
            break;
    }
    // clear the reference
    promotion_piece = nullptr;
}

/**
 * Post move cleanup before the next round can be setup.
 * This includes updating if a pawn moved this round or if there was a capture.
 *
 * Should be run immediately after the player makes a move
 */
void HexChessDriver::RoundCleanup() {
    CheckForPromotion();
    // 50 move rule updates
    if (!capture_this_round) {
        last_capture_round++;
    }
    else {
        // there was a capture this round
        last_capture_round = 0;
    }
    if (!pawn_moved) {
        last_pawn_move_round++;
    }
    else {
        // a pawn did move this turn
        last_pawn_move_round = 0;
    }
}

/**
 * Returns a vector of the coordinates of all pieces with valid moves.
 */
void HexChessDriver::GetSelectableTiles() {
    vector<ChessPiece>* pieces = current_player == WhitePlayer ? &white_pieces : &black_pieces;
    KingPiece* king = current_player == WhitePlayer ? &white_king : &black_king;
    active_selection.clear();
    active_moves.clear();
    // print list of all pieces with moves
    if (king->valid_moves_this_turn.size() > 0) {
        // add location to active selection list
        active_selection.push_back(king->GetLocation());
    }
    for (int i=0; i<pieces->size(); i++) {
        if ((*pieces)[i].valid_moves_this_turn.size() > 0) {
            // add location to active selection list
            active_selection.push_back((*pieces)[i].GetLocation());
        }
    }
}

/**
 * Clears the selected piece.
 *
 * Usage:
 *  1 - GetSelectableTiles() // get a list of selectable tiles
 *  2 - GetMoveTiles()       // get the list of valid moves from selected tile
 *  3 - ClearSelection()     // player wants to make a different selection
 *  4 - GetSelectableTiles() // show the list of selectable tiles again
 *  5 - GetMoveTiles()       // get the list of valid moves from the new selected tile
 *  5 - MovePiece()          // commit to a move
 */
void HexChessDriver::ClearSelection() {
    selected_piece = nullptr;
}

/**
 * Checks castling conditions and adds moves to king if met
 */
void HexChessDriver::AddCastlingMoves() {
    KingPiece* king = current_player == WhitePlayer ? &white_king : &black_king;
    if (!king->is_unmoved) {
        return; // no castling
    }
    if (resolver.IsInCheck(king)) {
        return; // no castling if king is in check
    }
    // ### Kingside ###
    // locations of the rooks and paths
    sCoords kingside_rook = current_player == WhitePlayer ? sCoords(3, i) : sCoords(10, i);
    sCoords kingside_t1 = current_player == WhitePlayer ? sCoords(1, g) : sCoords(10, g);
    sCoords kingside_t2 = current_player == WhitePlayer ? sCoords(2, h) : sCoords(10, h);
    vector<Tile*> kingside_path = {chessboard.GetTile(kingside_t1), chessboard.GetTile(kingside_t2)};
    // check contditions and add moves
    if (CastlingValidOnSide(kingside_rook, kingside_path, current_player)) {
        // add kingside castling to king's valid move set
        king->valid_moves_this_turn.push_back(kingside_rook);
        total_valid_moves_current_player++;
    }

    // ### Queenside ###
    // locations of the rooks and paths
    sCoords queenside_rook = current_player == WhitePlayer ? sCoords(0, c) : sCoords(7, c);
    sCoords queenside_t1 = current_player == WhitePlayer ? sCoords(0, d) : sCoords(8, d);
    sCoords queenside_t2 = current_player == WhitePlayer ? sCoords(0, e) : sCoords(9, e);
    vector<Tile*> queenside_path = {chessboard.GetTile(queenside_t1), chessboard.GetTile(queenside_t2)};
    // check contditions and add moves
    if (CastlingValidOnSide(queenside_rook, queenside_path, current_player)) {
        // add queenside castling to king's valid move set
        king->valid_moves_this_turn.push_back(queenside_rook);
        total_valid_moves_current_player++;
    }
}

/**
 * Checks if the given castling rook and path meet all conditions required to allow castling.
 */
bool HexChessDriver::CastlingValidOnSide(sCoords rook_loc, vector<Tile*>& path, ePlayer player) {
    // check for unmoved rook of same player at the starting location
    Tile* rook_tile = chessboard.GetTile(rook_loc);
    // rook tile must exist
    if (rook_tile == nullptr) {
        return false;
    }
    ChessPiece* tile_piece = rook_tile->GetPiece();
    // rook tile must contain the rook for this player
    if (tile_piece == nullptr) {
        return false;
    }
    // piece must be alive, unmoved, a rook, and same player
    if (!tile_piece->is_alive ||
        !tile_piece->is_unmoved ||
        tile_piece->type != Rook ||
        tile_piece->player != player
        ) {
        return false;
    }
    // rook tile can't be threatened
    if (resolver.IsThreatened(rook_tile, player)) {
        return false;
    }
    // every tile in the path must be empty and unthreatened
    for (Tile* tile : path) {
        if (tile->GetPiece() != nullptr) {
            return false;
        }
        if (resolver.IsThreatened(tile, player)) {
            return false;
        }
    }
    // all checks passed
    return true;
}

/**
 * Given the location of a selected piece, returns a vector of all valid move locations.
 */
void HexChessDriver::GetMoveTiles(int rank, int file) {
    sCoords selection = sCoords(rank, static_cast<eFiles>(file));
    // update the selected piece
    selected_piece = chessboard.GetTile(selection)->GetPiece();
    active_moves.clear();
    active_selection.clear();
    // check selected piece is valid
    if (selected_piece != nullptr) {
        for (sCoords move : selected_piece->valid_moves_this_turn) {
            // add move to active moves vector
            active_moves.push_back(move);
        }
    }
}

#include <bits/stdc++.h>
/**
 * moves the currently selected piece to the given location.
 *
 * Must follow immediately after GetMoveTiles since it requires the selected piece to be updated.
 */
void HexChessDriver::MovePiece(int rank, int file) {
    selected_piece->is_unmoved = false;
    // coordinates of target location
    sCoords target_loc = sCoords(rank, static_cast<eFiles>(file));
    // tile of target location
    Tile* end_tile = chessboard.GetTile(target_loc);
    // piece on target location
    ChessPiece* target = end_tile->GetPiece();
    // starting tile
    Tile* start_tile = chessboard.GetTile(selected_piece->GetLocation());
    
    // check for a capture
    if (target != nullptr) {
        // check for castling
        if (selected_piece->type == King &&
            target->type == Rook &&
            target->player == selected_piece->player) {
            // this is a castling move, update rook first
            int rank_diff = rank - selected_piece->GetLocation().rank;
            int file_diff = file - selected_piece->GetLocation().file;
            sCoords rook_new_loc = sCoords(0, a); // default location, will be overwritten
            // diffs will be:
            // black: kingside=0,3 queenside=-3,-3
            // white: kingside=3,3 queenside=0,-3
            if (file_diff > 0) {
                // kingside
                if (rank_diff == 0) {
                    // black
                    rook_new_loc = sCoords(rank, static_cast<eFiles>(file - 1));
                }
                else {
                    // white
                    rook_new_loc = sCoords(rank - 1, static_cast<eFiles>(file - 1));
                }
            }
            else {
                // queenside
                if (rank_diff == 0) {
                    // white
                    rook_new_loc = sCoords(rank, static_cast<eFiles>(file + 1));
                }
                else {
                    // black
                    rook_new_loc = sCoords(rank + 1, static_cast<eFiles>(file + 1));
                }
            }
            // move rook
            Tile* new_rook_tile = chessboard.GetTile(rook_new_loc);
            new_rook_tile->SetPiece(target);
            target->SetLocation(rook_new_loc);
            target->is_unmoved = false;
        }
        else {
            // moving piece will capture
            // remove from list of enemy player pieces
            target->is_alive = false;
            // update captures stat for this round
            capture_this_round = true;
        }
    }
    else { 
        // target is empty
        // check for en passant capture
        if (selected_piece->type == Pawn && target_loc == resolver.GetEnPassantLocation()) {
            // en passant capture, remove en passant piece from board
            chessboard.GetTile(en_passant_piece->GetLocation())->RemovePiece();
            // remove from list of enemy player pieces
            en_passant_piece->is_alive = false;
            // clear en passant piece
            en_passant_piece = nullptr;
            // set en passant location to an invalid value
            resolver.SetEnPassantLocation(sCoords(-1, a));
            // update captures stat for this round
            capture_this_round = true;
        }
        else {
            // check for pawn double move, can't have a double move and a en passant capture on the same round
            // with the hex board, diagonal movement also has a dela rank of 2
            int delta_rank = target_loc.rank - selected_piece->GetLocation().rank;
            int delta_file = target_loc.file - selected_piece->GetLocation().file;
            if (delta_rank < 0) { delta_rank = -delta_rank; } // absolute value of delta_rank
            if (selected_piece->type == Pawn && delta_rank == 2 && delta_file == 0) {
                // double move, add en passant piece
                en_passant_piece = selected_piece;
                int offset = current_player == WhitePlayer ? 1 : -1; // offset of en passant location from moving pawns starting location
                resolver.SetEnPassantLocation(selected_piece->GetLocation() + sRelCoords(offset, 0));
            }
        }
    }
    // remove piece from tile
    start_tile->RemovePiece();
    // set piece at new tile
    end_tile->SetPiece(selected_piece);
    // update piece location
    selected_piece->SetLocation(target_loc);
    // update if a pawn was moved
    pawn_moved = selected_piece->type == Pawn;
}

/**
 * Checks if the given coordinates are contained in the given vector of coordinates.
 */
bool HexChessDriver::CoordsInCoordsVector(sCoords* test_coords, vector<sCoords>* test_vector_coords) {
    for (sCoords x : *test_vector_coords) {
        if (x == *test_coords) {
            // match
            return true; 
        }
    }
    return false;
}

/**
 * Converts the given vector of Tile pointers to a vector of coordinates.
 */
vector<sCoords> HexChessDriver::TranslateVector(vector<Tile*>& vec) {
    vector<sCoords> translate_vec;
    for (int i=0; i<vec.size(); i++) {
        translate_vec.push_back(vec[i]->GetLocation());
    }
    return translate_vec;
}
