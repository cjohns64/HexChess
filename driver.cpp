#include "includes/board.h"
#include "includes/util.h"

enum eGameEnd {
    Checkmate = 0,
    Stalemate = 1,
    DeadPosistion = 2,
    ThreefoldRepitition = 3,
    FiftyMoveRule = 4
};

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
class Driver {
    private:
        // initial piece locations defined explicitly
        sCoords inital_placement[52] = { // White
            sCoords(0, f), // King
            sCoords(0, e), // Queen
            sCoords(1, e), sCoords(1, f), sCoords(1, g), // Bishops
            sCoords(0, d), sCoords(2, g), sCoords(2, h), // Knights
            sCoords(0, c), sCoords(3, i), // Rooks
            // Pawns
            sCoords(0, a), sCoords(0, b), sCoords(1, b), sCoords(1, c),
            sCoords(1, d), sCoords(2, d), sCoords(2, e), sCoords(2, f),
            sCoords(3, f), sCoords(3, g), sCoords(3, h), sCoords(4, h),
            sCoords(4, i), sCoords(4, j), sCoords(5, j), sCoords(5, k),
            // Black
            sCoords(10, f), // King
            sCoords(9, e), // Queen
            sCoords(8, e), sCoords(9, f), sCoords(10, g), // Bishops
            sCoords(8, d), sCoords(9, g), sCoords(10, h), // Knights
            sCoords(7, c), sCoords(10, i), // Rooks
            // Pawns
            sCoords(5, a), sCoords(5, b), sCoords(6, b), sCoords(6, c),
            sCoords(6, d), sCoords(7, d), sCoords(7, e), sCoords(7, f),
            sCoords(8, f), sCoords(8, g), sCoords(8, h), sCoords(9, h),
            sCoords(9, i), sCoords(9, j), sCoords(10, j), sCoords(10, k)
        };

        // White: 0a-0f, 1g, 2h, 3i, 4j, 5k
        sCoords back_rank_white[11] = {
            sCoords(0, a), sCoords(0, b), sCoords(0, c), sCoords(0, d), sCoords(0, e), sCoords(0, f),
            sCoords(1, g), sCoords(2, h), sCoords(3, i), sCoords(4, j), sCoords(5, k)
        };
        // Black: 10f-10k, 5a, 6b, 7c, 8d, 9e
        sCoords back_rank_black[11] = {
            sCoords(10, f), sCoords(10, g), sCoords(10, h), sCoords(10, i), sCoords(10, j), sCoords(10, k),
            sCoords(5, a), sCoords(6, b), sCoords(7, c), sCoords(8, d), sCoords(9, e)
        };
        int round_number = -1; // first step in game loop will increment this to 0
        int last_pawn_move_round = 0;
        int last_capture_round = 0;
        int total_valid_moves_current_player = 0;
        ePlayer current_player = WhitePlayer;
        // the actual pieces are stored here, all other classes use pointers to these pieces
        vector<ChessPiece> white_pieces;
        vector<ChessPiece> black_pieces;
        // store the Kings separately since they will need to be accessed for every move, and they can't be removed from the board
        KingPiece white_king;
        KingPiece black_king;
        // data for testing threats without hardcoding different moves
        sCoords empty_location = sCoords(0, a);
        // these pieces are only used for checking for threats and are never placed on the board
        vector<ChessPiece> test_pieces_white = {
            KingPiece(WhitePlayer, &empty_location),
            QueenPiece(WhitePlayer, &empty_location),
            RookPiece(WhitePlayer, &empty_location),
            KnightPiece(WhitePlayer, &empty_location),
            BishopPiece(WhitePlayer, &empty_location),
            PawnPiece(WhitePlayer, &empty_location)
        };
        // there is a version for the other player because pawns move in opposite directions
        // all pieces are included in case moves are different for the players.
        vector<ChessPiece> test_pieces_black = {
            KingPiece(BlackPlayer, &empty_location),
            QueenPiece(BlackPlayer, &empty_location),
            RookPiece(BlackPlayer, &empty_location),
            KnightPiece(BlackPlayer, &empty_location),
            BishopPiece(BlackPlayer, &empty_location),
            PawnPiece(BlackPlayer, &empty_location)
        };
        // temporarily point to a piece, for evaluating valid moves that involve capturing other pieces.
        ChessPiece* test_capture_piece = NULL;
        // A pointer to the currently selected piece
        ChessPiece* selected_piece = NULL;
        // A pointer to the piece that may be captured by en-passant, only valid for one opponent turn.
        // Doesn't matter if the opponent overwrites this value since it would have been cleared first thing after opponent's turn.
        ChessPiece* en_passant_piece = NULL;

        // vector of the pieces white lost
        vector<ChessPiece> white_dead_pieces;
        // vector of the pieces black lost
        vector<ChessPiece> black_dead_pieces;

        Board chessboard = Board();

    public:
        Driver() : white_king(KingPiece(WhitePlayer, &inital_placement[0])),
                   black_king(KingPiece(BlackPlayer, &inital_placement[26])) {
            // initial placement -- White
            for (int i=1; i<26; i++) {
                if (i>=10) {
                    white_pieces.push_back(PawnPiece(WhitePlayer, &inital_placement[i]));
                }
                else if (i>1 && i<=4) {
                    white_pieces.push_back(BishopPiece(WhitePlayer, &inital_placement[i]));
                }
                else if (i>5 && i<=7) {
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
                if (i>=36) {
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
         * Loop:
         * - update round number
         * - update current player = round number % 2
         * - update en-passant | if piece is same player, clear
         * - get valid moves for all current player pieces and save/count results in pieces
         * - check for win/draw
         * - player-turn
         *
         * Return value is the game over state
         */
        eGameEnd GameLoop() {
            while (true) {
                // update round number
                round_number++;
                // set the current player
                current_player = static_cast<ePlayer>(round_number % 2);
                // skip checking NULL en passant pieces
                if (en_passant_piece != NULL) {
                    // clear the en_passant_piece if it has made it back to the player that used it
                    if (en_passant_piece->player == current_player) {
                        en_passant_piece = NULL;
                    }
                }
                // check King
                KingPiece *king = current_player == WhitePlayer ? &white_king : &black_king;
                // resolve all moves
                vector<Tile*> resolved_moves;
                ResolveMoves(*king, resolved_moves);
                // validate all resolved moves
                ValidateResolvedMoves(resolved_moves, king);
                // clear total valid moves and update with number of valid moves for the king
                total_valid_moves_current_player = resolved_moves.size();
                // update all valid moves for piece
                king->valid_moves_this_turn = TranslateVector(resolved_moves);
                

                // check Pieces
                vector<ChessPiece> *pieces = current_player == WhitePlayer ? &white_pieces : &black_pieces;

                for (int i=0; i<pieces->size(); i++) {
                    // resolve all moves
                    vector<Tile*> resolved_moves;
                    ResolveMoves((*pieces)[i], resolved_moves);

                    // validate all resolved moves
                    ValidateResolvedMoves(resolved_moves, &(*pieces)[i]);
                    // add valid moves to total valid moves count
                    total_valid_moves_current_player += resolved_moves.size();
                    // update all valid moves for piece
                    (*pieces)[i].valid_moves_this_turn = TranslateVector(resolved_moves);
                }
                // check if total number of moves > 0
                if (total_valid_moves_current_player == 0) {
                    // STALEMATE OR CHECKMATE
                    if (IsInCheck(king)) {
                        return Checkmate;
                    }
                    return Stalemate;
                }
                // check for win/draw conditions
                if (last_pawn_move_round >= 50 && last_pawn_move_round >= 50) {
                    // 50 move rule
                    return FiftyMoveRule;
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
                    return DeadPosistion;
                }

                // ask the player for a move
                PlayerTurn();
            }
        }

    private:
        /**
         * Turn:
         * - select piece
         * - get valid moves from saved set
         * - select? ... undo/redo? ... move?
         * - update game log
         */
        void PlayerTurn() {
            //TODO selection
            //TODO repeat until a move is made
            //TODO move
        }

        vector<sCoords> TranslateVector(vector<Tile*>& vec) {
            vector<sCoords> translate_vec;
            for (int i=0; i<vec.size(); i++) {
                translate_vec.push_back(vec[i]->GetLocation());
            }
            return translate_vec;
        }

        /**
         * Converts the move set of a given piece to a vector of tiles that the piece can physically move to.
         * Does not check if the move will put the King in check, only that the tiles are unobstructed or a capture can be made.
         */
        void ResolveMoves(ChessPiece& piece, vector<Tile*>& resolved_moves) {
            // piece location
            sCoords location = piece.GetLocation();
            // check initial moves
            if (piece.is_unmoved) {
                for (int i=0; i<piece.inital_moves.size(); i++) {
                    sRelCoords move = piece.inital_moves[i];
                    ResolveSingleRelMove(move, location, resolved_moves, piece.captures_with_moves, piece.player);
                }
            }
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
                    ResolveSingleRelMove(move, location, resolved_moves, false, piece.player);
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
        void ResolveSingleRelMove(sRelCoords& move, sCoords& location, vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer, bool is_test=false) {
            Tile* tile;
            if (!move.repeat) {
                // check move is available
                tile = chessboard.GetTile(location + move);
                ResolveThisMove(tile, resolved_moves, can_capture, player, is_test);
            }
            else {
                int x=1;
                do {
                    // check all repeat moves in the direction given by moves
                    tile = chessboard.GetTile(location + (move * x));
                    x++;
                } while (!ResolveThisMove(tile, resolved_moves, can_capture, player, is_test));
            }
        }

        /**
         * Resolves the fetched tile, returns true if the tile was the end of a repeat pattern,
         * and false if a repeat move may still exist.
         */
        bool ResolveThisMove(Tile* tile, vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer, bool is_test=false){
            // check the tile is on the board
            if (tile != nullptr) {
                // check if the tile is empty
                if (tile->GetPiece(is_test) == NULL) {
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
        void GetThreatened(vector<ChessPiece*>& threats, sCoords location, ePlayer player, bool return_immediately=false, bool is_test=false) {
            // get the test pieces. The move sets could be different, so get the other player's pieces
            vector<ChessPiece>* test_pieces = player == BlackPlayer ? &test_pieces_white : &test_pieces_black;
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
                        if (test_threat != NULL) {
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
         * Finds if the given King is in check.
         */
        bool IsInCheck(KingPiece* king, bool is_test=false) {
            vector<ChessPiece*> threats;
            GetThreatened(threats, king->GetLocation(), king->player, true, is_test);
            return threats.size() > 0;
        }

        /**
         * Filters the resolved_moves vector to only contain moves that do not put the king in check.
         */
        void ValidateResolvedMoves(vector<Tile*>& resolved_moves, ChessPiece* moving_piece) {
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
        bool ValidateMove(sCoords move, ChessPiece* moving_piece) {
            // test move piece to location
            // get the destination tile
            Tile* move_tile = chessboard.GetTile(move);
            // get the home tile
            Tile* home_tile = chessboard.GetTile(moving_piece->GetLocation());
            // set destination tile to have the test piece
            move_tile->TestPieceSet(moving_piece);
            // set the home tile to not have the testing piece
            home_tile->TestPieceStash();

            // get king for this player
            KingPiece* king = moving_piece->player == WhitePlayer ? &white_king : &black_king;
            // see if king is in check now
            bool bad_move = IsInCheck(king, true);

            // clear test moves
            move_tile->TestPieceRemove();
            home_tile->TestPieceRestore();
            return !bad_move;
        }

        bool CheckForPromotion(ChessPiece* piece) {
            if (piece->is_promotable) {
                // get the set of promotion coordinates
                sCoords* back_rank = piece->player == WhitePlayer ? back_rank_black : back_rank_white; 
                sCoords location = piece->GetLocation();
                // check for a match
                for (int i=0; i<11; i++) {
                    if (back_rank[i] == location) {
                        return true;
                    }
                }
            }
            return false;
        }

};


int main() {
    return 0;
}
