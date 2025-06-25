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
