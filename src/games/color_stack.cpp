
struct CSCell {
    bool occupied;
    vec2i gridPosition;
};

struct CSPiece {
    int32 colorIndex;
    vec2 position;
    vec2 velocity;

    bool atRest;

    int32 colDest;
    int32 rowDest;
};

struct CSColumn {
    DynamicArray<CSCell> cells;
};

struct ColorStackData : GameData {
    Sprite dotSprite;
    Sprite ringSprite;

    int32 rowCount;
    DynamicArray<CSColumn> columns;

    int32 currentPiece;
    DynamicArray<CSPiece> pieces;

    DynamicArray<SoundClip> sounds;

    DynamicArray<SoundClip> fallingSounds;

    DynamicArray<int32> fallingSoundIndices;
    int32 fallingSoundIndex;

    DynamicArray<SoundClip> topSounds;
    int32 topSoundIndex;

    float32 columnTimer;
    int32 columnIndex;
};

void ColorStackInit() {
    ColorStackData *Data = (ColorStackData *)GM.gameData;

    TileWidth = 8;
    TileHeight = 8;

    GridWidth = 7;
    GridHeight = 8;

    SetMosaicGridSize(TileWidth * GridWidth,
                      TileHeight * GridHeight);

    Data->columns = MakeDynamicArray<CSColumn>(&GM.gameArena, GridWidth);

    Data->rowCount = GridHeight - 1;

    For (c, GridWidth) {
        CSColumn col = {};
        col.cells = MakeDynamicArray<CSCell>(&GM.gameArena, GridHeight);

        For (r, Data->rowCount) {
            CSCell cell = {};
            cell.gridPosition = V2i(c, Data->rowCount - r);
            // @NOTE: storing cells from bottom to top
            
            PushBack(&col.cells, cell);
        }

        PushBack(&Data->columns, col);
    }

    {
        CSPiece piece = {};
        piece.colorIndex = 0;
        piece.position = V2(0, 0);
        piece.velocity = V2(0);

        Data->pieces = MakeDynamicArray<CSPiece>(&GM.gameArena, 128);

        PushBack(&Data->pieces, piece);
    }

    LoadSpriteFromFile("data/sprites/ring_frame1.png", &Data->ringSprite, &GM.gameArena);
    LoadSpriteFromFile("data/sprites/dot1.png", &Data->dotSprite, &GM.gameArena);

    {
        Data->sounds = MakeDynamicArray<SoundClip>(&GM.gameArena, 16);
        
        // LoadSoundClip("data/sfx/flute/flute_c3.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute/flute_e3.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute/flute_f3.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute/flute_g3.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute/flute_b3.wav", PushBackPtr(&Data->sounds));

        //LoadSoundClip("data/sfx/flute/flute_c4.wav", PushBackPtr(&Data->sounds));
        
        // LoadSoundClip("data/sfx/flute_short/flute_c4.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute_short/flute_e4.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute_short/flute_g4.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute_short/flute_c5.wav", PushBackPtr(&Data->sounds));
        // LoadSoundClip("data/sfx/flute_short/flute_e5.wav", PushBackPtr(&Data->sounds));

        LoadSoundClip("data/sfx/marimba/Marimba_ff_C4.wav", PushBackPtr(&Data->sounds));
        LoadSoundClip("data/sfx/marimba/Marimba_ff_E4.wav", PushBackPtr(&Data->sounds)); 
        LoadSoundClip("data/sfx/marimba/Marimba_ff_G4.wav", PushBackPtr(&Data->sounds));
        LoadSoundClip("data/sfx/marimba/Marimba_ff_C5.wav", PushBackPtr(&Data->sounds));
        LoadSoundClip("data/sfx/marimba/Marimba_ff_E5.wav", PushBackPtr(&Data->sounds)); 
    }

    {
        Data->fallingSoundIndices = MakeDynamicArray<int32>(&GM.gameArena, 7);
        PushBack(&Data->fallingSoundIndices, 0);
        PushBack(&Data->fallingSoundIndices, 1);
        PushBack(&Data->fallingSoundIndices, 2);
        PushBack(&Data->fallingSoundIndices, 3);
        PushBack(&Data->fallingSoundIndices, 4);
        PushBack(&Data->fallingSoundIndices, 2);
        PushBack(&Data->fallingSoundIndices, 4);
    }

    {
        Data->topSounds = MakeDynamicArray<SoundClip>(&GM.gameArena, 16);
        
        LoadSoundClip("data/sfx/flute/flute_c3.wav", PushBackPtr(&Data->topSounds));
        LoadSoundClip("data/sfx/flute/flute_g3.wav", PushBackPtr(&Data->topSounds));
        LoadSoundClip("data/sfx/flute/flute_c3.wav", PushBackPtr(&Data->topSounds));
        LoadSoundClip("data/sfx/flute/flute_e3.wav", PushBackPtr(&Data->topSounds));
    }
}

void ColorStackUpdate() {
    ColorStackData *Data = (ColorStackData *)GM.gameData;

    Data->columnTimer += DeltaTime;

    real32 rate = 1.0f;
    if (Data->columnTimer >= rate) {
        Data->columnTimer -= rate;
        Data->columnIndex = (Data->columnIndex + 1) % GridWidth;

        CSPiece *piece = &Data->pieces[Data->pieces.count - 1];
        piece->position = TilePositionToPixel(Data->columnIndex, 0);

        //PlaySound(Data->topSounds[RandiRange(0, Data->topSounds.count)], 0.3f);
        PlaySound(Data->topSounds[Data->topSoundIndex], 0.3f);
        Data->topSoundIndex = (Data->topSoundIndex + 1) % Data->topSounds.count;
    }

    if (InputPressed(Gamepad, Input_FaceA) ||
        InputPressed(Gamepad, Input_FaceB) ||
        InputPressed(Keyboard, Input_Space)) {
        PlaySound(Data->sounds[RandiRange(0, Data->sounds.count)], 1.0f);
        
        CSPiece *piece = &Data->pieces[Data->pieces.count - 1];
        piece->velocity = V2(0, 40);

        int32 rowIndex = -1;
        int32 columnIndex = Data->columnIndex;
        For (r, Data->columns[columnIndex].cells.count) {
            CSCell *cell = &Data->columns[columnIndex].cells[r];

            if (cell->occupied) {
                
                continue;
            }
            else {
                rowIndex = r;
                // @WARNING: this does not mean the piece is actually there yet, just that
                // there will be a piece there once it's fallen.
                cell->occupied = true;
                break;
            }
        }

        if (rowIndex < 0) {
            piece->velocity = V2(0);
        }
        else {
            piece->colDest = columnIndex;
            piece->rowDest = rowIndex;

            {
                CSPiece piece = {};
                piece.colorIndex = RandiRange(0, Data->columns.count);
                piece.position = TilePositionToPixel(Data->columnIndex, 0);
                piece.velocity = V2(0);

                PushBack(&Data->pieces, piece);
            }
        }
    }

    For (i, Data->pieces.count) {
        CSPiece *piece = &Data->pieces[i];

        vec2 prevPosition = piece->position;
        piece->position = piece->position + piece->velocity * DeltaTime;

        int32 columnIndex = piece->colDest;
        For (r, Data->columns[columnIndex].cells.count) {
            CSCell *cell = &Data->columns[columnIndex].cells[r];
            vec2 cellPosition = TilePositionToPixel(cell->gridPosition.x, cell->gridPosition.y);

            if (piece->position.y >= cellPosition.y && prevPosition.y < cellPosition.y) {
                PlaySound(Data->sounds[RandiRange(0, Data->sounds.count)], 0.3f);

                int32 index = Data->fallingSoundIndices[Data->fallingSoundIndex];
                Data->fallingSoundIndex = (Data->fallingSoundIndex + 1) % Data->fallingSoundIndices.count;
                //PlaySound(Data->sounds[index], 0.5f);
            }
        }

        CSCell *destCell = &Data->columns[piece->colDest].cells[piece->rowDest];
        vec2 cellPosition = TilePositionToPixel(destCell->gridPosition.x, destCell->gridPosition.y);
        
        if (piece->position.y >= cellPosition.y && !piece->atRest) {
            piece->atRest = true;
            //PlaySound(Data->sounds[RandiRange(0, Data->sounds.count)], 1.0f);
            
            piece->velocity = V2(0);
            piece->position = cellPosition;
        }
    }

    vec4 colors[] = {
                     V4(1, 0, 0, 1),
                     V4(1.0f, 0.4f, 0.0f, 1.0f),
                     V4(1.0, 0.85f, 0.0f, 1.0f),
                     V4(0, 1, 0, 1),
                     V4(0, 0, 1, 1),
                     V4(0.5, 0.0f, 0.5f, 1.0f),
                     V4(0.8, 0.0f, 0.8f, 1.0f),
    };

    For (i, Mosaic->tileCapacity) {
        MTile *tile = &Mosaic->tiles[i];
        SetTileSprite(V2(tile->position.x, tile->position.y), 0);
        
        tile->scale = 1.8f + (0.3f * sinf(Time + (i * 0.1f)));
        tile->rotation = (Time * 0.2f) + (i * 0.1f);
        tile->color = V4(0);
    }

    For (i, Data->pieces.count) {
        CSPiece *piece = &Data->pieces[i];
        MDrawSpriteData spriteData = {};
        spriteData.color = colors[piece->colorIndex];
        spriteData.spriteIndex = 1;

        MDrawSprite(piece->position, &Data->dotSprite, spriteData, 0);
    }

    For (c, Data->columns.count) {
        vec2i gridCursor = V2i(c, 1);
        vec2 cursor = TilePositionToPixel(gridCursor.x, gridCursor.y);
        
        For (r, Data->rowCount) {
            MDrawSpriteData spriteData = {};
            spriteData.color = colors[c];
            spriteData.colorB = colors[c] * 0.5f;
            spriteData.spriteIndex = 0;
            
            MDrawSprite(cursor, &Data->ringSprite, colors[c], 1);
            
            gridCursor.y++;
            cursor = TilePositionToPixel(gridCursor.x, gridCursor.y);
        }
    }

    // MDrawSprite(V2(20, 10), &Data->ringSprite, V4(0, 1, 0, 1));
}
