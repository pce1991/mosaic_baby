
struct CLDot {
    vec2 position;
};

struct ChristmasLightsData : GameData {
    Sprite dotSprite;

    DynamicArray<CLDot> dots;
};

void ChristmasLightsInit() {
    ChristmasLightsData *Data = (ChristmasLightsData*)GM.gameData;

    TileWidth = 8;
    TileHeight = 8;

    GridWidth = 4;
    GridHeight = 4;

    SetMosaicGridSize(TileWidth * GridWidth,
                      TileHeight * GridHeight);

    LoadSpriteFromFile("data/sprites/dot1.png", &Data->dotSprite, &GM.gameArena);

    {
        Data->dots = MakeDynamicArray<CLDot>(&GM.gameArena, 8);
        
        int32 columns = 4;
        int32 rows = 4;

        real32 padding = Mosaic->gridWidth / columns;

        For (c, columns) {
            vec2 cursor = V2(8 + (c * padding), 0);

            if (c % 2 == 0) {
                cursor.y += 12;
            }
 
            For (r, rows) {
                CLDot dot = {};
                dot.position = cursor;

                PushBack(&Data->dots, dot);

                cursor.y += padding;
            }
        }
    }
}

void ChristmasLightsUpdate() {
    ChristmasLightsData *Data = (ChristmasLightsData*)GM.gameData;

    real32 speed = 10.0f;
    For (i, Data->dots.count) {
        CLDot *dot = &Data->dots[i];

        dot->position.y += speed * DeltaTime;

        if (dot->position.y > Mosaic->gridHeight) {
            dot->position.y = 0 - TileHeight;
        }
    }

    For (y, Mosaic->gridHeight) {
        For (x, Mosaic->gridWidth) {
            real32 signedNoise = Sum1f(&Perlin1f, ((Time * 2.0 + ((x + y * 19) * 0.5f)) * 0.2f), 1.0f, 1.0f);
            real32 normNoise = (1 + signedNoise) * 0.5f;
            
            vec2 pos = V2(x, y);

            vec4 colorA = V4(0.1f, 0.3f, 0.7f, 0.3f);
            vec4 colorB = V4(0.1f, 0.4f, 0.5f, 0.4f);

            // vec4 colorA = V4(0.1f, 0.1f, 0.1f, 1.0f);
            // vec4 colorB = V4(0.05f, 0.05f, 0.05f, 1.0f);

            vec4 color = Lerp(colorA, colorB, normNoise);
            SetTileColor(pos, WithAlpha(color, 0.5f));
            //SetTileColorB(pos, WithAlpha(color * 0.2f, 1.0f));
            SetTileColorB(pos, V4(0));
            
            SetTileRotation(pos, (x + (y * 19)) * _PI * 0.08f);
            SetTileScale(pos, 2.0 + (normNoise * 0.9f));
            //SetTileScale(pos, 1.0f);
            SetTileSprite(pos, 1);
            SetTileDepth(pos, 0);

            // @BUG: whenever we set tile data we want to specify a depth otherwise it's
            // goign to take on whatever is already there
        }
    }

    For (i, Data->dots.count) {
        CLDot *dot = &Data->dots[i];

        //MDrawSprite(dot->position, &Data->dotSprite);
    }

    // @TODO: pass in parameters along with the sprite for scale/rot/sprite/depth
    MDrawSprite(V2(12, 12), &Data->dotSprite);
}
