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
        int round_number = 0;
        int last_pawn_move_round = 0;
        int last_capture_round = 0;
        ePlayer current_player = WhitePlayer;
        std::vector<ChessPiece> white_pieces;
        KingPiece white_king;
        std::vector<ChessPiece> black_pieces;
        KingPiece black_king;
        // temporarily point to a piece, for evaluating valid moves that involve capturing other pieces.
        ChessPiece* test_capture_piece;
        // A pointer to the currently selected piece
        ChessPiece* selected_piece;
        // A pointer to the piece that may be captured by en-passant, only valid for one opponent turn.
        // Doesn't matter if the opponent overwrites this value since it would have been cleared first thing after opponent's turn.
        ChessPiece* en_passant_piece;

        // vector of the pieces white lost
        std::vector<ChessPiece> white_dead_pieces;
        // vector of the pieces black lost
        std::vector<ChessPiece> black_dead_pieces;

        Board chessboard = Board();

    public:
        Driver() {
            //TODO initialize vars
        }

};

int main() {
    return 0;
}
