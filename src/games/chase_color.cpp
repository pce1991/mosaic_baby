void ChaseColorInit();
void ChaseColorUpdate();

struct ChaseColorData : GameData {


};

void ChaseColorInit() {
    SetMosaicGridSize(80, 60);
}

void ChaseColorUpdate() {

    //Mosaic->screenColor = V4(0.55, 0.55f, 0.45f, 1.0f);
    Mosaic->screenColor = V4(0.2f);

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            
            // SetTileColor(x, y,
            //              sinf(Time + (x * y)),
            //              sinf(Time + 0.4f + (x * y)),
            //              sinf(Time + 0.8f + (y))
            //              );

            real32 xT = x / (Mosaic->gridWidth * 1.0f);
            real32 yT = y / (Mosaic->gridHeight * 1.0f);
            
            real32 r = Lerp(0.25f, 0.4f, xT * xT);
            real32 b = Lerp(0.7f, 0.14f, yT);

            SetTileColor(x, y,
                         r,
                         //sinf(Time + 0.4f + (x * y)),
                         0.2f * cosf(y * 0.1f),
                         b
                         );
        }
    }

    MDrawSprite(V2(40 + (sinf(Time) * 20), 30), &GM.heroSprite);

    if (InputPressed(Keyboard, Input_Space)) {
        SetActiveGame(GameID_HerdColor);
    }
}
