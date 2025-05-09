#include "board.cpp"

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
        int round_number = -1; // first step in game loop will increment this to 0
        int last_pawn_move_round = 0;
        int last_capture_round = 0;
        ePlayer current_player = WhitePlayer;
        // the actual pieces are stored here, all other classes use pointers to these pieces
        std::vector<ChessPiece> white_pieces;
        std::vector<ChessPiece> black_pieces;
        // store the Kings separately since they will need to be accessed for every move, and they can't be removed from the board
        KingPiece white_king;
        KingPiece black_king;
        // data for testing threats without hardcoding different moves
        sCoords empty_location = sCoords(0, a);
        // these pieces are only used for checking for threats and are never placed on the board
        std::vector<ChessPiece> test_pieces_white = {
            KingPiece(WhitePlayer, &empty_location),
            QueenPiece(WhitePlayer, &empty_location),
            RookPiece(WhitePlayer, &empty_location),
            KnightPiece(WhitePlayer, &empty_location),
            BishopPiece(WhitePlayer, &empty_location),
            PawnPiece(WhitePlayer, &empty_location)
        };
        // there is a version for the other player because pawns move in opposite directions
        // all pieces are included in case moves are different for the players.
        std::vector<ChessPiece> test_pieces_black = {
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
        std::vector<ChessPiece> white_dead_pieces;
        // vector of the pieces black lost
        std::vector<ChessPiece> black_dead_pieces;

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
         */
        void GameLoop() {
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
            std::vector<Tile*> resolved_moves;
            ResolveMoves(*king, resolved_moves);
            //TODO validate all resolved moves
            //TODO update all valid moves for piece

            // check Pieces
            std::vector<ChessPiece> *pieces = current_player == WhitePlayer ? &white_pieces : &black_pieces;

            for (int i=0; i<pieces->size(); i++) {
                // resolve all moves
                std::vector<Tile*> resolved_moves;
                ResolveMoves((*pieces)[i], resolved_moves);

                //TODO validate all resolved moves
                //TODO update all valid moves for piece
            }
            // TODO check if total number of moves > 0
            //TODO check for win/draw conditions
        }

    private:
        /**
         * Turn:
         * - select piece
         * - get valid moves from saved set
         * - select? ... undo/redo? ... move?
         * - update game log
         */
        void RunTurn() {
        }

        /**
         * Converts the move set of a given piece to a vector of tiles that the piece can physically move to.
         * Does not check if the move will put the King in check, only that the tiles are unobstructed or a capture can be made.
         */
        void ResolveMoves(ChessPiece& piece, std::vector<Tile*>& resolved_moves) {
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
        void ResolveSingleRelMove(sRelCoords& move, sCoords& location, std::vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer) {
            Tile* tile;
            if (!move.repeat) {
                // check move is available
                tile = chessboard.GetTile(location + move);
                ResolveThisMove(tile, resolved_moves, can_capture, player);
            }
            else {
                int x=1;
                do {
                    // check all repeat moves in the direction given by moves
                    tile = chessboard.GetTile(location + (move * x));
                    x++;
                } while (!ResolveThisMove(tile, resolved_moves, can_capture, player));
            }
        }

        /**
         * Resolves the fetched tile, returns true if the tile was the end of a repeat pattern,
         * and false if a repeat move may still exist.
         */
        bool ResolveThisMove(Tile* tile, std::vector<Tile*>& resolved_moves, bool can_capture=true, ePlayer player=WhitePlayer){
            // check the tile is on the board
            if (tile != nullptr) {
                // check if the tile is empty
                if (tile->GetPiece() == NULL) {
                    // add tile to resolved moves
                    resolved_moves.push_back(tile);
                    return false;
                }
                // check if the occupying piece is an enemy piece
                else if (can_capture && tile->GetPiece()->player != player){
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
        void GetThreatened(std::vector<ChessPiece*>& threats, sCoords location, ePlayer player, bool return_immediately=false) {
            // get the test pieces. The move sets could be different, so get the other player's pieces
            std::vector<ChessPiece>* test_pieces = player == BlackPlayer ? &test_pieces_white : &test_pieces_black;
            // set the position of the test pieces to the test position
            // this will set all the other test pieces since they all point to the same location
            (*test_pieces)[0].SetLocation(location);
            // check each piece in the test piece vector
            for (int i=0; i<test_pieces->size(); i++) {
                // get a piece
                ChessPiece* piece = &(*test_pieces)[i];
                // setup resolver vector for this piece
                std::vector<Tile*> resolved_moves;
                // resolve each capture move
                for (int j=0; j<piece->captures.size(); j++) {
                    // check each capture move one at a time and invert their direction
                    sRelCoords move = piece->captures[j].invert();
                    // resolve moves with other player's inverted move set but with this player's color.
                    // will find all of the other player's pieces that can capture this player's color at the test location.
                    ResolveSingleRelMove(move, location, resolved_moves, true, player);
                    // clear out results that are empty
                    for (int x=0; x<resolved_moves.size(); x++) {
                        ChessPiece* test_threat = resolved_moves[x]->GetPiece();
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
        bool IsInCheck(KingPiece* king) {
            std::vector<ChessPiece*> threats;
            GetThreatened(threats, king->GetLocation(), king->player, true);
            return threats.size() > 0;
        }

};

int main() {
    return 0;
}
