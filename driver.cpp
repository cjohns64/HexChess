#include <iostream>
#include "includes/board.h"
#include "includes/util.h"
#include "includes/resolver.h"

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
        bool pawn_moved = false;
        int last_capture_round = 0;
        bool capture_this_round = false;
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
        Resolver resolver;

    public:
        Driver() : white_king(KingPiece(WhitePlayer, &inital_placement[0])),
                   black_king(KingPiece(BlackPlayer, &inital_placement[26])),
                   resolver(Resolver(&chessboard, &white_king, &black_king, &test_pieces_white, &test_pieces_white))
                   {
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
                        return Checkmate;
                    }
                    return Stalemate;
                }
                // check for win/draw conditions
                if (last_capture_round >= 50 || last_pawn_move_round >= 50) {
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

                // 50 move rule setup
                capture_this_round = false;
                pawn_moved = false;

                // ask the player for a move
                PlayerTurn(current_player);

                // 50 move rule updates
                if (!capture_this_round) {
                    last_capture_round++;
                }
                else {
                    last_capture_round = 0;
                }
                if (!pawn_moved) {
                    last_pawn_move_round++;
                }
                else {
                    last_pawn_move_round = 0;
                }
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
        void PlayerTurn(ePlayer player) {
            //TODO print the board state
            // reference this player's pieces
            vector<ChessPiece>* pieces = player == WhitePlayer ? &white_pieces : &black_pieces;
            KingPiece* king = player == WhitePlayer ? &white_king : &black_king;

            //TODO selection
            // Ask for a selection
            cout << "Select a piece:\n";
            // print list of all pieces with moves
            if (king->valid_moves_this_turn.size() > 0) {
                cout << "King:" << ToString(king->GetLocation()) << " ";
            }
            for (int i=0; i<pieces->size(); i++) {
                if ((*pieces)[i].valid_moves_this_turn.size() > 0) {
                    // print type:location for each piece with valid moves
                    cout << ToString((*pieces)[i].type) << ":" << ToString((*pieces)[i].GetLocation()) << " ";
                }
            }
            // TODO get input
            // print possible moves for piece
            // Ask for a move or a different selection
            // get input
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
};


int main() {
    return 0;
}
