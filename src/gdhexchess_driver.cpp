#include "includes/gdhexchess_driver.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDHexChessDriver::_bind_methods() {
    ClassDB::bind_method(D_METHOD("RoundSetup"), &GDHexChessDriver::RoundSetup);
    ClassDB::bind_method(D_METHOD("RoundCleanup"), &GDHexChessDriver::RoundCleanup);
    ClassDB::bind_method(D_METHOD("GetGameState"), &GDHexChessDriver::GetGameState);
    ClassDB::bind_method(D_METHOD("ClearSelection"), &GDHexChessDriver::ClearSelection);
    ClassDB::bind_method(D_METHOD("MovePiece", "rank", "file"), &GDHexChessDriver::MovePiece);
    ClassDB::bind_method(D_METHOD("GetPieceOnTile", "rank", "file"), &GDHexChessDriver::GetPieceOnTile);
    ClassDB::bind_method(D_METHOD("GetSelectableTiles"), &GDHexChessDriver::GetSelectableTiles);
    ClassDB::bind_method(D_METHOD("GetMoveTiles", "rank", "file"), &GDHexChessDriver::GetMoveTiles);
    ClassDB::bind_method(D_METHOD("GetActionOnTile", "rank", "file"), &GDHexChessDriver::GetActionOnTile);
    ClassDB::bind_method(D_METHOD("GetPromotionTile"), &GDHexChessDriver::GetPromotionTile);
    ClassDB::bind_method(D_METHOD("RunPromotion", "selection"), &GDHexChessDriver::RunPromotion);
}


GDHexChessDriver::GDHexChessDriver() : driver(HexChessDriver()) {}
int GDHexChessDriver::GetPieceOnTile(int rank, int file) { return driver.GetPieceOnTile(rank, file); }
int GDHexChessDriver::GetActionOnTile(int rank, int file) { return driver.GetActionOnTile(rank, file); }
void GDHexChessDriver::RoundSetup() { driver.RoundSetup(); }
int GDHexChessDriver::GetGameState() { return driver.GetGameState(); }
void GDHexChessDriver::RoundCleanup() { driver.RoundCleanup(); }
void GDHexChessDriver::GetSelectableTiles() { driver.GetSelectableTiles(); }
void GDHexChessDriver::ClearSelection() { driver.ClearSelection(); }
void GDHexChessDriver::GetMoveTiles(int rank, int file) { driver.GetMoveTiles(rank, file); }
void GDHexChessDriver::MovePiece(int rank, int file) { driver.MovePiece(rank, file); }
int GDHexChessDriver::GetPromotionTile() { return driver.GetPromotionTile(); }
void GDHexChessDriver::RunPromotion(int selection) { driver.RunPromotion(selection); }
