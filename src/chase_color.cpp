void ChaseColorInit();
void ChaseColorUpdate();

struct ChaseColorData : GameData {

    // Okay how do we automatically call this initializer? 
    ChaseColorData() {
        init = &ChaseColorInit;
        update = &ChaseColorUpdate;
    }
};

void ChaseColorInit() {
    SetMosaicGridSize(16, 16);
}

void ChaseColorUpdate() {
    SetTileColor(0, 0, 1, 0.2f, 0.2f);
}
