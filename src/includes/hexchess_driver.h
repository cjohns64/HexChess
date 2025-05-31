#ifndef HEXCHESSDRIVER_H
#define HEXCHESSDRIVER_H
#include "board.h"
#include "util.h"
#include "resolver.h"
#include <vector>

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
    // A pointer to the currently selected piece
    ChessPiece* selected_piece = nullptr;
    // A pointer to the piece that may be captured by en-passant, only valid for one opponent turn.
    // Doesn't matter if the opponent overwrites this value since it would have been cleared first thing after opponent's turn.
    ChessPiece* en_passant_piece = nullptr;

    // vector of the pieces white lost
    vector<ChessPiece> white_dead_pieces;
    // vector of the pieces black lost
    vector<ChessPiece> black_dead_pieces;

    Board chessboard = Board();
    Resolver resolver;

public:
    HexChessDriver();

    /**
     * Returns the piece type on the given location 
     * OR NoPiece if tile is not on the board or no piece is on the tile.
     */
    eType GetPieceTypeOnTile(sCoords location);

    /**
     * Returns the player that owns the piece at the given location 
     * defaults to White if no piece is on the tile.
     */
    ePlayer GetPlayerAtLocation(sCoords location);

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
    void RoundSetup();

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
    vector<sCoords> GetSelectableTiles();

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
    vector<sCoords> GetMoveTiles(sCoords selection);

    /**
     * moves the currently selected piece to the given location.
     *
     * Must follow immediately after GetMoveTiles since it requires the selected piece to be updated.
     */
    void MovePiece(sCoords target_loc);

private:
    /**
     * Checks if the given coordinates are contained in the given vector of coordinates.
     */
    bool CoordsInCoordsVector(sCoords* test_coords, vector<sCoords>* test_vector_coords);

    /**
     * Converts the given vector of Tile pointers to a vector of coordinates.
     */
    vector<sCoords> TranslateVector(vector<Tile*>& vec);
};
#endif
