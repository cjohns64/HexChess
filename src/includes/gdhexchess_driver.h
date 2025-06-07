#ifndef GDHEXCHESSDRIVER_H
#define GDHEXCHESSDRIVER_H
#include "hex_chess_driver.h"

#include <godot_cpp/classes/node.hpp>

namespace godot {

/**
 * Adds Godot specific Node functions to HexChessDriver
 */
class GDHexChessDriver: public Node {
    GDCLASS(GDHexChessDriver, Node)

private:
    HexChessDriver driver;
protected:
	static void _bind_methods();
public:
    GDHexChessDriver();

    int GetPieceOnTile(int rank, int file);
    int GetActionOnTile(int rank, int file);
    void RoundSetup();
    int GetGameState();
    void RoundCleanup();
    void GetSelectableTiles();
    void ClearSelection();
    void GetMoveTiles(int rank, int file);
    void MovePiece(int rank, int file);

protected:
    bool CoordsInCoordsVector(sCoords* test_coords, vector<sCoords>* test_vector_coords);
    vector<sCoords> TranslateVector(vector<Tile*>& vec);
};
}
#endif
