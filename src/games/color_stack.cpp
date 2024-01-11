
struct CSCell {
    bool occupied;
    vec2i position;
}; 

struct CSColumn {
    DynamicArray<CSCell> cells;
};

struct ColorStackData : GameData {
    Sprite dotSprite;
    Sprite ringSprite;

    int32 rowCount;
    int32 currColumn;
    DynamicArray<CSColumn> columns;
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
            cell.position = V2i(c, r);
            
            PushBack(&col.cells, cell);
        }

        PushBack(&Data->columns, col);
    }

    LoadSpriteFromFile("data/sprites/ring1.png", &Data->ringSprite, &GM.gameArena);
    LoadSpriteFromFile("data/sprites/dot1.png", &Data->dotSprite, &GM.gameArena);
}

void ColorStackUpdate() {
    ColorStackData *Data = (ColorStackData *)GM.gameData;

    vec4 colors[] = {
                     V4(1, 0, 0, 1),
                     V4(1.0f, 0.5f, 0.0f, 1.0f),
                     V4(1, 1, 0, 1),
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

    For (c, Data->columns.count) {
        vec2i gridCursor = V2i(c, 1);
        vec2 cursor = TilePositionToPixel(gridCursor.x, gridCursor.y);
        
        For (r, Data->rowCount) {
            MDrawSprite(cursor, &Data->ringSprite, colors[c]);
            
            gridCursor.y++;
            cursor = TilePositionToPixel(gridCursor.x, gridCursor.y);
        }
    }

    {        
        MDrawSpriteData spriteData = {};
        spriteData.color = V4(1, 0, 0, 1);
        spriteData.spriteIndex = 1;
    
        MDrawSprite(V2(0, 0), &Data->dotSprite, spriteData);
    }

    // MDrawSprite(V2(20, 10), &Data->ringSprite, V4(0, 1, 0, 1));
}
