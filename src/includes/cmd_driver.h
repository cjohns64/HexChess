#ifndef CMDHEXCHESSDRIVER_H
#define CMDHEXCHESSDRIVER_H
#include "hex_chess_driver.h"
#include <iostream>

/**
 * Adds Godot specific Node functions to HexChessDriver
 */
class CmdHexChessDriver: public HexChessDriver {
public:
    void PrintBoard(); 
};
#endif
