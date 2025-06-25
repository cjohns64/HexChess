#ifndef HEXCHESSDRIVER_H
#define HEXCHESSDRIVER_H
#include "board.h"
#include "util.h"
#include "resolver.h"
#include <vector>
#include <map>


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
class HexChessDriver {

protected:
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
    const sCoords back_rank_white[11] = {
        sCoords(0, a), sCoords(0, b), sCoords(0, c), sCoords(0, d), sCoords(0, e), sCoords(0, f),
        sCoords(1, g), sCoords(2, h), sCoords(3, i), sCoords(4, j), sCoords(5, k)
    };
    // Black: 10f-10k, 5a, 6b, 7c, 8d, 9e
    const sCoords back_rank_black[11] = {
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
    eGameEnd game_state = Running;
    // the actual pieces are stored here, all other classes use pointers to these pieces
    vector<ChessPiece> white_pieces;
    vector<ChessPiece> black_pieces;
    // store the Kings separately since they will need to be accessed for every move, and they can't be removed from the board
    KingPiece white_king;
    KingPiece black_king;
    
    const sCoords null_location = sCoords(-1, a);
    // data for testing threats without hardcoding different moves
    sCoords empty_location = sCoords(0, a);
    // these pieces are only used for checking for threats and are never placed on the board
    vector<ChessPiece> test_pieces_white = {
        KingPiece(WhitePlayer, empty_location),
        QueenPiece(WhitePlayer, empty_location),
        RookPiece(WhitePlayer, empty_location),
        KnightPiece(WhitePlayer, empty_location),
        BishopPiece(WhitePlayer, empty_location),
        PawnPiece(WhitePlayer, empty_location)
    };
    // there is a version for the other player because pawns move in opposite directions
    // all pieces are included in case moves are different for the players.
    vector<ChessPiece> test_pieces_black = {
        KingPiece(BlackPlayer, empty_location),
        QueenPiece(BlackPlayer, empty_location),
        RookPiece(BlackPlayer, empty_location),
        KnightPiece(BlackPlayer, empty_location),
        BishopPiece(BlackPlayer, empty_location),
        PawnPiece(BlackPlayer, empty_location)
    };
    // A pointer to the currently selected piece
    ChessPiece* selected_piece = nullptr;
    // A pointer to the piece that may be captured by en-passant, only valid for one opponent turn.
    // Doesn't matter if the opponent overwrites this value since it would have been cleared first thing after opponent's turn.
    ChessPiece* en_passant_piece = nullptr;

    // working selection and moves
    vector<sCoords> active_selection;
    vector<sCoords> active_moves;
    vector<Tile*> white_promotion_tiles;
    vector<Tile*> black_promotion_tiles;
    // this holds the piece that can be promoted to a higher value piece
    ChessPiece* promotion_piece = nullptr;

    Board chessboard = Board();
    Resolver resolver;
    vector<string> position_log; // log of board state, each row is a full game board just before white's turn
    map<string, int> position_histogram;

public:
    HexChessDriver();

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
    int GetPieceOnTile(int rank, int file);
    
    /**
     * Returns the avalible action type for a given location 
     *
     * Data is encoded as an int where:
     *      0 no action
     *      1 selectable piece
     *      2 move
     *      3 move & selectable piece
     */
    int GetActionOnTile(int rank, int file);

    /**
     * - update round number
     * - update current player = round number % 2
     * - update en-passant | if piece is same player, clear
     * - get valid moves for all current player pieces and save/count results in pieces
     * - check for win/draw
     *
     * Should be run before the player is allowed to select a piece
     */
    void RoundSetup();

    /**
     * Return value is the game over state
     */
    int GetGameState();

    /**
     * Post move cleanup before the next round can be setup.
     * This includes updating if a pawn moved this round or if there was a capture.
     *
     * Should be run immediately after the player makes a move
     */
    void RoundCleanup();

    /**
     * Returns a vector of the coordinates of all pieces with valid moves.
     */
    void GetSelectableTiles();

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
    void ClearSelection();

    /**
     * Given the location of a selected piece, returns a vector of all valid move locations.
     */
    void GetMoveTiles(int rank, int file);

    /**
     * moves the currently selected piece to the given location.
     *
     * Must follow immediately after GetMoveTiles since it requires the selected piece to be updated.
     */
    void MovePiece(int rank, int file);

    /**
     * Returns the location of the promotable piece or 0 if there is none.
     * The location is encoded as a two digit hex number stored as a single decimal number as follows:
     *  rank 0-10 -> 0x1 - 0xb
     *  file a-k  -> 0x1 - 0xb
     *  Return value: rank file -> 0x11 - 0xbb -> 17 - 187
     */
    int GetPromotionTile();

    /**
     * Promotes the current promotion piece to the given selection.
     * Selection is an int of the following form:
     *  2 = Rook
     *  3 = Bishop
     *  4 = Knight
     *  Any other value = Queen
     */
    void RunPromotion(int piece_selection);

protected:
    /**
     * Checks if the given coordinates are contained in the given vector of coordinates.
     */
    bool CoordsInCoordsVector(sCoords* test_coords, vector<sCoords>* test_vector_coords);

    /**
     * Converts the given vector of Tile pointers to a vector of coordinates.
     */
    vector<sCoords> TranslateVector(vector<Tile*>& vec);

    /**
     * Checks castling conditions and adds moves to king if met
     */
    void AddCastlingMoves();

    /**
     * Checks if the given castling rook and path meet all conditions required to allow castling.
     */
    bool CastlingValidOnSide(sCoords rook_loc, vector<Tile*>& path, ePlayer player);

    /**
     * Checks each promotion tile for a valid promotion, and updates the promotion piece if their is one.
     */
    void CheckForPromotion();

    /**
     * checks the position_histogram for positions that have occurred 3 or more times.
     */
    bool CheckForThreefoldRepetition();

    /**
     * logs the content of each tile on the board into a compressed bit format stored in the position_log vector,
     * one tile is stored as follows:
     * 0b0000 = No value
     * 0b0001 = black king
     * 0b0010 = black queen
     * 0b0011 = black rook
     * 0b0100 = black bishop
     * 0b0101 = black knight
     * 0b0110 = black pawn
     * 0b0111   -- not used --
     * 0b1000   -- not used --
     * 0b1001 = white king
     * 0b1010 = white queen
     * 0b1011 = white rook
     * 0b1100 = white bishop
     * 0b1101 = white knight
     * 0b1110 = white pawn
     * 0b1111 = empty tile
     *
     * Each row of the position_log vector is 23 elements long with each element containing 4 tiles.
     * Each tile is stored in order from 0a -> 10k = 0 -> 90,
     * and each row of the position_log vector is a full game board.
     */
    void LogGamePosition();
};
#endif
