void HerdColorInit();
void HerdColorUpdate();

struct HerdColorData : GameData {


};

void HerdColorInit() {
    SetMosaicGridSize(16, 16);
}

void HerdColorUpdate() {
    SetTileColor(0, 0, 0.2, 0.2f, 1.0f);
}
