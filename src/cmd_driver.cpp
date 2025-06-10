#include "includes/cmd_driver.h"

void CmdHexChessDriver::PrintBoard() {
    for (int i=0; i<11; i++) {
        for (int j=0; j<11; j++) {
            Tile* x = chessboard.GetTile(sCoords(i, static_cast<eFiles>(j)));
            if (x != nullptr) {
                ChessPiece* t = x->GetPiece();
                if (t == nullptr) {
                    // empty tile
                    cout << "[    ]" << " ";
                }
                else {
                    char p = t->type != Knight ? ToString(t->type)[0] : 'N';
                    cout << "[" << p;
                    cout << " " << ToString(t->GetLocation()) << "]" << " ";
                }
            }
        }
        cout << endl;
    }
}


int main() {
    /** moves for incorrect column bug
    * rank=1 file=2
    * rank=3 file=2
    * rank=6 file=2
    * rank=4 file=2
    * rank=2 file=3
    * rank=3 file=3
    * rank=4 file=2
    * rank=3 file=3
    * rank=3 file=2
    * rank=3 file=5
    * rank=5 file=6
    *
    * moves for pawn promotion
    * rank=0 file=0
    * rank=2 file=0
    * rank=5 file=1
    * rank=4 file=1
    * rank=2 file=0
    * rank=4 file=1
    * rank=6 file=3
    * rank=4 file=3
    * rank=4 file=1
    * rank=5 file=0
    *
    * moves for a unexplained crash
    * rank=0 file=0
    * rank=2 file=0
    * rank=6 file=2
    * rank=4 file=2
    * rank=0 file=3
    * rank=2 file=2
    * rank=7 file=2
    * rank=5 file=2
    * rank=2 file=0
    * rank=3 file=0
    */
    CmdHexChessDriver driver = CmdHexChessDriver();
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
    int vec[] = {0, 0,  // 
                2, 0,   // 
                6, 2,   // 
                4, 2,   // 
                0, 3,   // 
                2, 2, // 
                7, 2, // 
                5, 2, // 
                2, 0, // 
                3, 0  // 
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
        if (i == 16) {
            driver.RunPromotion(0);
        }
    }
    return 0;
}
