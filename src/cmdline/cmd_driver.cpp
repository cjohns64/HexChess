#include "../includes/cmd_driver.h"

void CmdHexChessDriver::PrintBoard() {
    for (int i=0; i<11; i++) {
        // add spacing for shorter rows
        for (int n=0; n<abs(5-i); n++) {
            cout << "   ";
        }
        for (int j=0; j<11; j++) {
            Tile* x = chessboard.GetTile(sCoords(i, static_cast<eFiles>(j)));
            if (x != nullptr) {
                ChessPiece* t = x->GetPiece();
                // print empty tiles with spaces
                if (t == nullptr) {
                    // empty tile
                    cout << "{    }" << "";
                }
                // print a filled tile
                else {
                    char p = t->type != Knight ? ToString(t->type)[0] : 'N';
                    char c = t->player == WhitePlayer ? 'w' : 'b';
                    cout << "{" << p;
                    cout << c << ToString(t->GetLocation()) << "}" << "";
                }
            }
        }
        cout << endl;
    }
}


int main() {
    CmdHexChessDriver driver = CmdHexChessDriver();
    // incorrect column bug
    // int vec[] = {1, 2,  // select w pawn 1
    //             3, 2,   // double move pawn 1
    //             6, 2,   // select black pawn A
    //             4, 2,   // double move pawn A
    //             2, 3,   // select w pawn 2
    //             3, 3, // single move pawn 2
    //             4, 2, // select black pawn A
    //             3, 3, // capture w pawn 2
    //            // 3, 2, // select w pawn 1
    //             3, 5, // select w pawn 3
    //             5, 6};// bad move with pawn 3
    // pawn promotion
    // int vec[] = {0, 0,  // select w pawn 1
    //             2, 0,   // double move pawn 1
    //             5, 1,   // select black pawn A
    //             4, 1,   // single move pawn A
    //             2, 0,   // select w pawn 1
    //             4, 1, // capture black pawn A
    //             6, 3, // select a black piece
    //             4, 3, // move the black piece
    //             4, 1, // select w pawn 1
    //             5, 0,// capture black pawn 2, promotion
    //             9, 9, // select b pawn B
    //             7, 9,// double move b pawn B
    //             2, 6, // select w knight 1, //check promoted piece's move resolving and validation on [5, a]
    //             5, 8};// move w knight
    // crash at unknown
    // int vec[] = {
    //             1,1,
    //             3,1,
    //             6,2,
    //             5,2,
    //             3,1,
    //             5,2,
    //             7,2,
    //             5,2,
    //             0,3,
    //             2,2
    // };
    // int vec[] = {0, 0,  // 
    //             2, 0,   // 
    //             6, 2,   // 
    //             4, 2,   // 
    //             0, 3,   // 
    //             2, 2, // 
    //             7, 2, // 
    //             5, 2, // 
    //             2, 0, // 
    //             3, 0  // 
    //             };
    // crash when castling
    // int vec[] = {
    //              1 ,1,
    //              3 ,1,
    //              5 ,0,
    //              4 ,0,
    //              3 ,1,
    //              4 ,0,
    //              8 ,3,
    //              6 ,4,
    //              4 ,0,
    //              5 ,0,
    //              9 ,6,
    //              6 ,5,
    //              5 ,0,
    //              5 ,1,
    //              6 ,4,
    //              5 ,1,
    //              0 ,1,
    //              2 ,1,
    //              5 ,1,
    //              2 ,0,
    //              0 ,3,
    //              2 ,2,
    //              6 ,1,
    //              4 ,1,
    //              2 ,7,
    //              4 ,6,
    //              6 ,5,
    //              4 ,6,
    //              2 ,5,
    //              4 ,6,
    //              2 ,0,
    //              4 ,3,
    //              1 ,6,
    //              4 ,3,
    //              7 ,2,
    //              5 ,0,
    //              0 ,5,
    //              3 ,8,
    //              };
    // en passant
    // int vec[] = {
    //              1 ,1,
    //              3 ,1,
    //              6 ,2,
    //              4 ,2
    //              };
    //  faulty en passant capture
    int vec[] = {
            0 ,0,
            2 ,0,
            5 ,1,
            3 ,1,
            2 ,0,
            4 ,1,
            5 ,0,
            3 ,0,
                 };


    int n = sizeof(vec) / sizeof(vec[0]);
    for (int i=0; i<n; i++) {
        if (i%4 != 0) {
            continue;
        }
        driver.PrintBoard();
        driver.RoundSetup();
        driver.GetSelectableTiles();
        cout << "Selection:: " << vec[i+0] << ":" << vec[i+1] << endl;
        //driver.ClearSelection();
        //driver.GetSelectableTiles();
        driver.GetMoveTiles(vec[i+0], vec[i+1]);
        driver.MovePiece(vec[i+2], vec[i+3]);
        cout << "Move:: " << vec[i+2] << ":" << vec[i+3] << endl;
        driver.RoundCleanup();
        // if (i == 16) {
        //     driver.RunPromotion(0);
        // }
    }
    driver.PrintBoard();
    driver.RoundSetup();
    driver.GetSelectableTiles();
    return 0;
}
