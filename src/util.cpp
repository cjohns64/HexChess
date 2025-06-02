#include "includes/util.h"
sCoords::sCoords(int _rank, eFiles _file){
        rank = _rank;
        file = _file;
}

bool sCoords::operator == (const sCoords& obj) {
    // overload == for compairing coordinates
    return rank == obj.rank && file == obj.file;
}

sRelCoords::sRelCoords(int _rel_rank, int _rel_file, bool _repeat, eType _target_requirement, eMoveReq _tile_requirement, bool _initial_only) {
    rel_rank = _rel_rank;
    rel_file = _rel_file;
    repeat = _repeat;
    target_requirement = _target_requirement;
    tile_requirement = _tile_requirement;
    initial_only = _initial_only;
}

/**
 * Returns the relative movement in the opposite direction
 */
sRelCoords sRelCoords::invert() {
    return sRelCoords(-rel_rank, -rel_file, repeat);
}

// overload == for compairing rel coordinates
bool sRelCoords::operator == (const sRelCoords& obj) {
    return rel_rank == obj.rel_rank && rel_file == obj.rel_file && repeat == obj.repeat;
}
