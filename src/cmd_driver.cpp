#include "includes/cmd_driver.h"

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
void HexChessDriver::GetSelectableTiles() {
    // set the current player
    ePlayer player = static_cast<ePlayer>(round_number % 2);
    vector<ChessPiece>* pieces = player == WhitePlayer ? &white_pieces : &black_pieces;
    KingPiece* king = player == WhitePlayer ? &white_king : &black_king;
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
    // return active_selection;
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
void HexChessDriver::GetMoveTiles(int rank, int file) {
    sCoords selection = sCoords(rank, static_cast<eFiles>(file));
    // update the selected piece
    selected_piece = chessboard.GetTile(selection)->GetPiece();
    active_moves.clear();
    active_selection.clear();
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
    //return active_moves;
}

#include <bits/stdc++.h>
/**
 * moves the currently selected piece to the given location.
 *
 * Must follow immediately after GetMoveTiles since it requires the selected piece to be updated.
 */
void HexChessDriver::MovePiece(int rank, int file) {
    sCoords target_loc = sCoords(rank, static_cast<eFiles>(file));
    // move piece
    ChessPiece* target = chessboard.GetTile(target_loc)->GetPiece();
    //ChessPiece* selected_piece = chessboard.GetTile(selection_loc)->GetPiece();
    // check for a capture
    if (target != nullptr) {
        // moving piece will capture
        // remove from list of player pieces
        target->is_alive = false;
        // update captures stat for this round
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

void HexChessDriver::PrintBoard() {
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
            Tile* x = chessboard.GetTile(sCoords(i, static_cast<eFiles>(j)));
            if (x != nullptr) {
                ChessPiece* t = x->GetPiece();
                if (t == nullptr) {
                    // empty tile
                    cout << "[    ]" << " ";
                }
                else {
                    char p = t->type != Knight ? ToString(t->type)[0] : 'N';
                    cout << "[" << p;
                    cout << " " << ToString(t->GetLocation()) << "]" << " ";
                }
            }
        }
        cout << endl;
    }
}


#include <iostream>
int main() {
    /** moves for incorrect column bug
    * rank=1 file=2
    * rank=3 file=2
    * rank=6 file=2
    * rank=4 file=2
    * rank=2 file=3
    * rank=3 file=3
    * rank=4 file=2
    * rank=3 file=3
    * rank=3 file=2
    * rank=3 file=5
    * rank=5 file=6
    */
    HexChessDriver driver = HexChessDriver();
    int vec[] = {1, 2,  // select w pawn 1
                3, 2,   // double move pawn 1
                6, 2,   // select black pawn A
                4, 2,   // double move pawn A
                2, 3,   // select w pawn 2
                3, 3, // single move pawn 2
                4, 2, // select black pawn A
                3, 3, // capture w pawn 2
               // 3, 2, // select w pawn 1
                3, 5, // select w pawn 3
                5, 6};// bad move with pawn 3
    for (int i=0; i<20; i++) {
        if (i%4 != 0) {
            continue;
        }
        driver.RoundSetup();
        driver.GetSelectableTiles();
        //driver.ClearSelection();
        //driver.GetSelectableTiles();
        driver.GetMoveTiles(vec[i+0], vec[i+1]);
        driver.MovePiece(vec[i+2], vec[i+3]);
        driver.RoundCleanup();
    }
    return 0;
}
