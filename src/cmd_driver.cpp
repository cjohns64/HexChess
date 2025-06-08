#include "includes/cmd_driver.h"

void CmdHexChessDriver::PrintBoard() {
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
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
    */
    CmdHexChessDriver driver = CmdHexChessDriver();
    int vec[] = {1, 2,  // select w pawn 1
                3, 2,   // double move pawn 1
                6, 2,   // select black pawn A
                4, 2,   // double move pawn A
                2, 3,   // select w pawn 2
                3, 3, // single move pawn 2
                4, 2, // select black pawn A
                3, 3, // capture w pawn 2
               // 3, 2, // select w pawn 1
                3, 5, // select w pawn 3
                5, 6};// bad move with pawn 3
    for (int i=0; i<20; i++) {
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
    }
    return 0;
}
