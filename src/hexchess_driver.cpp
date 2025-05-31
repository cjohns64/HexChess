#include "includes/hexchess_driver.h"

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
HexChessDriver::HexChessDriver() : white_king(KingPiece(WhitePlayer, &inital_placement[0])),
           black_king(KingPiece(BlackPlayer, &inital_placement[26])),
           resolver(Resolver(&chessboard, &white_king, &black_king, &test_pieces_white, &test_pieces_white))
           {
    // initial placement -- White
    for (int i=1; i<26; i++) {
        if (i>9) {
            white_pieces.push_back(PawnPiece(WhitePlayer, &inital_placement[i]));
        }
        else if (i>1 && i<=4) {
            white_pieces.push_back(BishopPiece(WhitePlayer, &inital_placement[i]));
        }
        else if (i>4 && i<=7) {
            white_pieces.push_back(KnightPiece(WhitePlayer, &inital_placement[i]));
        }
        else if (i>7 && i<=9) {
            white_pieces.push_back(RookPiece(WhitePlayer, &inital_placement[i]));
        }
        else if (i==1) {
            white_pieces.push_back(QueenPiece(WhitePlayer, &inital_placement[i]));
        }
    }
    // initial placement -- Black 
    for (int i=27; i<52; i++) {
        if (i>35) {
            black_pieces.push_back(PawnPiece(BlackPlayer, &inital_placement[i]));
        }
        else if (i>27 && i<=30) {
            black_pieces.push_back(BishopPiece(BlackPlayer, &inital_placement[i]));
        }
        else if (i>30 && i<=33) {
            black_pieces.push_back(KnightPiece(BlackPlayer, &inital_placement[i]));
        }
        else if (i>33 && i<=35) {
            black_pieces.push_back(RookPiece(BlackPlayer, &inital_placement[i]));
        }
        else if (i==27) {
            black_pieces.push_back(QueenPiece(BlackPlayer, &inital_placement[i]));
        }
    }
    // set the board with the actual pieces, the board will store pointers to each piece on their current Tile.
    chessboard.SetBoard(white_pieces, white_king, black_pieces, black_king);
}

/**
 * Returns the piece type on the given location 
 * OR NoPiece if tile is not on the board or no piece is on the tile.
 */
eType HexChessDriver::GetPieceTypeOnTile(sCoords location) {
    Tile* tile = chessboard.GetTile(location);
    if (tile != nullptr && tile->GetPiece() != nullptr) {
        return tile->GetPiece()->type;
    }
    else {
        return NoPiece;
    }
}

/**
 * Returns the player that owns the piece at the given location 
 * defaults to White if no piece is on the tile.
 */
ePlayer HexChessDriver::GetPlayerAtLocation(sCoords location) {
    Tile* tile = chessboard.GetTile(location);
    if (tile != nullptr && tile->GetPiece() != nullptr) {
        return tile->GetPiece()->player;
    }
    else {
        return WhitePlayer;
    }
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
    for (int i=0; i<pieces->size(); i++) {
        if ((*pieces)[i].type == Pawn || (*pieces)[i].type == Rook || (*pieces)[i].type == Queen) {
            // Not comprehensive
            dead_position = false;
            break;
        }
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
 * Post move cleanup before the next round can be setup.
 * This includes updating if a pawn moved this round or if there was a capture.
 *
 * Should be run immediately after the player makes a move
 */
void HexChessDriver::RoundCleanup() {
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
vector<sCoords> HexChessDriver::GetSelectableTiles() {
    // set the current player
    ePlayer player = static_cast<ePlayer>(round_number % 2);
    vector<ChessPiece>* pieces = player == WhitePlayer ? &white_pieces : &black_pieces;
    KingPiece* king = player == WhitePlayer ? &white_king : &black_king;
    vector<sCoords> active_selection;
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
    return active_selection;
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
 * Given the location of a selected piece, returns a vector of all valid move locations.
 */
vector<sCoords> HexChessDriver::GetMoveTiles(sCoords selection) {
    // update the selected piece
    selected_piece = chessboard.GetTile(selection)->GetPiece();
    vector<sCoords> active_moves;
    // check selected piece is valid
    if (selected_piece != nullptr) {
        // print possible moves for piece
        for (sCoords move : selected_piece->valid_moves_this_turn) {
            // add move to active moves vector
            active_moves.push_back(move);
            // get a possible target piece
            ChessPiece* target = chessboard.GetTile(move)->GetPiece();
            // check if move is a capture
            if (selected_piece->type == Pawn && en_passant_piece != nullptr) {
                // check for en_passant
                // TODO
            }
        }
    }
    return active_moves;
}

#include <bits/stdc++.h>
/**
 * moves the currently selected piece to the given location.
 *
 * Must follow immediately after GetMoveTiles since it requires the selected piece to be updated.
 */
void HexChessDriver::MovePiece(sCoords target_loc) {
    // move piece
    ChessPiece* target = chessboard.GetTile(target_loc)->GetPiece();
    //ChessPiece* selected_piece = chessboard.GetTile(selection_loc)->GetPiece();
    // check for a capture
    if (target != nullptr) {
        // moving piece will capture
        vector<ChessPiece>* other_pieces = selected_piece->player == BlackPlayer ? &white_pieces : &black_pieces;
        vector<ChessPiece>* dead_pieces = selected_piece->player == BlackPlayer ? &white_dead_pieces : &black_dead_pieces;
        // add piece to captured pieces
        dead_pieces->push_back(*target);
        // remove from list of player pieces
        other_pieces->erase(find(other_pieces->begin(), other_pieces->end(), *target));
        capture_this_round = true;
    }
    // move the piece
    Tile* start_tile = chessboard.GetTile(selected_piece->GetLocation());
    Tile* end_tile = chessboard.GetTile(target_loc);
    // remove piece from tile
    start_tile->RemovePiece();
    // set piece at new tile
    end_tile->SetPiece(selected_piece);
    // update piece location
    selected_piece->SetLocation(target_loc);
    // update if a pawn was moved
    pawn_moved = selected_piece->type == Pawn;
    // check for pawn initial move if a pawn was moved
    if (pawn_moved) {
        // TODO add en_passant to valid moves, check this move was an initial move
        // vector<sCoords> initial_moves = move_selection.GetMovingPiece()->inital_moves;
        // if (CoordsInCoordsVector(&target_coords, &initial_moves) {
            // ;
        // }
    }
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

#include <iostream>
int main() {
    HexChessDriver driver = HexChessDriver();
    driver.RoundSetup();
    vector<sCoords> sel_vec = driver.GetSelectableTiles();
    cout << "Selection: " << ToString(sel_vec[0]) << endl;
    driver.ClearSelection();
    cout << "New Selection" << endl;
    sel_vec = driver.GetSelectableTiles();
    cout << "Selection: " << ToString(sel_vec[1]) << endl;
    vector<sCoords> mov_vec = driver.GetMoveTiles(sel_vec[1]);
    cout << "Move: " << ToString(mov_vec[0]) << endl;
    driver.MovePiece(mov_vec[0]);
    driver.RoundCleanup();
    return 0;
}
