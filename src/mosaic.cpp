
#include "my_mosaic.cpp"


// @NOTE: Some of this stuff is internal and you don't ever want it to change.
// Things like allocating the RectBuffer or calculating the levelAspect.
// Other things like setting the gridWidth can be customized.
void MyInit() {
    Game->myData = malloc(sizeof(MosaicMem));
    memset(Game->myData, 0, sizeof(MosaicMem));
    
    Mosaic = (MosaicMem *)Game->myData;

    MoveMouse(Game->screenWidth / 2.0f, Game->screenHeight / 2.0f);

    Mosaic->padding = 1.0f;

    SetMosaicGridSize(16, 16);

    Mosaic->screenColor = RGB(0.2f, 0.2f, 0.2f);
    Mosaic->gridColor = RGB(0.8f, 0.8f, 0.8f);

    AllocateTexturedTileBuffer(&Mosaic->tileRenderBuffer, 255 * 255);

    MyMosaicInit();
}


// @BUG: this doesnt guarantee that it fits inside the camera if the height is bigger than the width! 
void SetMosaicGridSize(uint32 newWidth, uint32 newHeight) {
    Mosaic->gridWidth = Clamp(newWidth, 1, 255);
    Mosaic->gridHeight = Clamp(newHeight, 1, 255);

    if (Mosaic->tiles != NULL) {
        free(Mosaic->tiles);
    }
    
    Mosaic->tileCapacity = Mosaic->gridWidth * Mosaic->gridHeight;
    Mosaic->tiles = (MTile*)malloc(sizeof(MTile) * Mosaic->tileCapacity);

    memset(Mosaic->tiles, 0, Mosaic->tileCapacity * sizeof(MTile));

    real32 levelAspect = Mosaic->gridWidth / (Mosaic->gridHeight * 1.0f);
    real32 screenAspect = 16.0 / 9.0f;
    // @HARDCODED

    Mosaic->tileSize = 1;

    // @TODO: keep a dedicated place at the top for text?
    {
        Camera *cam = &Game->camera;

        if (levelAspect > screenAspect) {
            real32 size = Mosaic->gridWidth / (16.0f - Mosaic->padding);
            
            cam->width = 16.0f * size;
            cam->height = 9.0f * size;
        }
        else {
            real32 size = Mosaic->gridHeight / (9.0f - Mosaic->padding);
            
            cam->width = 16.0f * size;
            cam->height = 9.0f * size;
        }
        
        cam->type = CameraType_Orthographic;
        // cam->width = 16;
        // cam->height = 9;
        cam->projection = Orthographic(cam->width * -0.5f, cam->width * 0.5f,
                                       cam->height * -0.5f, cam->height * 0.5f,
                                       0.0, 100.0f);

        mat4 camWorld = TRS(Game->cameraPosition, Game->cameraRotation, V3(1));
        cam->view = OrthogonalInverse(camWorld);
    
        cam->viewProjection = cam->projection * cam->view;

    }

    Mosaic->lineThickness = Mosaic->tileSize * 0.04f;

    // @TODO: add the line sizes
    Mosaic->gridSize.x = Mosaic->tileSize * Mosaic->gridWidth;
    Mosaic->gridSize.y = Mosaic->tileSize * Mosaic->gridHeight;
    
    Mosaic->gridOrigin = V2(0) + V2(-Mosaic->gridSize.x * 0.5f, Mosaic->gridSize.y * 0.5f);

    MTile*tiles = Mosaic->tiles;
    int32 index = 0;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            MTile*tile = GetTile(x, y);
            tile->index = index;
            tile->position = V2i(x, y);
            tile->rotation = 0;
            tile->scale = 1;
            index++;
        }
    }
}

void SetMosaicGridColor(vec4 color) {
    Mosaic->gridColor = color;
}

void SetMosaicGridColor(real32 r, real32 g, real32 b) {
    Mosaic->gridColor = RGB(r, g, b);
}

void SetMosaicScreenColor(vec4 color) {
    Mosaic->screenColor = color;
}

void SetMosaicScreenColor(real32 r, real32 g, real32 b) {
    Mosaic->screenColor = RGB(r, g, b);
}

void SetGridColor(vec4 color) {
    Mosaic->gridColor = color;
}

void SetGridColor(real32 r, real32 g, real32 b) {
    Mosaic->gridColor = RGB(r, g, b);
}

void ShowGrid() {
    Mosaic->drawGrid = true;
}

void HideGrid() {
    Mosaic->drawGrid = false;
}


int32 CellIndex(int32 x, int32 y) {
    return x + (y * Mosaic->gridWidth);
}

vec2 GridPositionToWorldPosition(vec2i gridPosition) {
    vec2 worldPos = Mosaic->gridOrigin;
    real32 size = Mosaic->tileSize;
    worldPos.x += (size * gridPosition.x) + (size * 0.5f);
    worldPos.y += (-size * gridPosition.y) + (-size * 0.5f);
    return worldPos;
}

void DrawTile(vec2i position, vec4 color) {
    vec2 worldPos = GridPositionToWorldPosition(position);

#if 0
    DrawRect(&Game->rectBuffer, worldPos, V2(Mosaic->tileSize * 0.5f), color);
#else 
    // @TODO: look up the texture on the tile

    int32 bokehIndex = (position.x + position.y) % GM.bokehMasks.count;
    //bokehIndex = 0;

    real32 scaledSize = Mosaic->tileSize * 0.55f;
    real32 scaleOffset = (1 + sinf((Time + position.x * position.y) * 0.5) * 0.5f) * 0.2f;

    real32 offsetRange = Mosaic->tileSize * 0.35f;
    vec2 posOffset = V2(sinf((Time + position.x + position.y) * 0.4f),
                        cosf((Time + position.x + position.y) * 0.4f)) * offsetRange;

    real32 rot = 0.0f;

    posOffset = V2(0);
    scaledSize = Mosaic->tileSize * 0.5f;
    scaleOffset = 0.0f;
    rot = 0;

#if STAINED_GLASS
    // @TODO: input some control over these outside the render loop
    posOffset = V2(0);
    scaledSize = Mosaic->tileSize * 0.5f;
    scaleOffset = 0.0f;
    rot = sinf((Time * 0.1f) + position.x + position.y) * _PI;

    posOffset = V2(0);
    scaledSize = Mosaic->tileSize * 1;
    scaleOffset = (1 + sinf((Time + position.x + position.y) * 0.5) * 0.5f) * 0.2;
    rot = sinf((Time * 0.1f) + position.x + position.y) * _PI;
    rot = (_PI * position.x + position.y) + (sinf(Time * 0.1f + position.x + position.y) * _PI * 0.1f);
#elif PAINT_1
    posOffset = V2(0);
    scaledSize = Mosaic->tileSize * 0.5f;
    scaleOffset = (1 + sinf((Time + position.x + position.y + color.r + color.g + color.b) * 0.5) * 0.5f) * 0.5;
    rot = 0;

#elif PAINT_2
    int32 index = position.x + (position.y * Mosaic->gridWidth);
    real32 noise = Sum1f(&Perlin1f, ((Time * 0.1f + index) * 0.2f), 1.0f, 1.0f);

    posOffset = V2(0);
    scaledSize = Mosaic->tileSize * 0.9;
    scaleOffset = 0;
    rot = noise * _PI;
#endif

    
    DrawMaskedQuad(worldPos + posOffset,
                   V2(scaledSize + scaleOffset),
                   // @TODO: allow a lil bit of overlap
                   rot,
                   
                   //0.0f,
                   color,
                   &GM.bokehMasks[bokehIndex]);
#endif
}

void DrawTile(MTile *tile) {
    vec2 worldPos = GridPositionToWorldPosition(tile->position);
    vec2 posOffset = V2(0);

    if (tile->sprite == NULL) {
        DrawRect(&Game->rectBuffer, worldPos, V2(Mosaic->tileSize * 0.5f), tile->color);
    }
    else {
        DrawMaskedQuad(worldPos + posOffset,
                       V2((Mosaic->tileSize * 0.5) * tile->scale),
                       tile->rotation,
                       tile->color,
                       tile->sprite);
    }
}

inline int32 MTile_Comparator(MTile const *a, MTile const *b) {
    // @NOTE: if they are using different sprites we cant defer to their position because
    // that would create more batches, so we sort by the sprite pointer
    if (a->depth < b->depth) {
        return -1;
    }
    else if (a->depth > b->depth) {
        return 1;
    }
    else {
        if (a->sprite < b->sprite) {
            return -1;
        }
        else if (a->sprite > b->sprite) {
            return 1;
        }
        
        if (a->index < b->index) {
            return -1;
        }
        if (a->index > b->index) {
            return 1;
        }
    }

    return 0;
}

void DrawBorder() {
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        if (y > 0 && y < Mosaic->gridHeight) { continue; }
        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f + (Mosaic->lineThickness), Mosaic->lineThickness), Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        if (x > 0 && x < Mosaic->gridWidth) { continue; }
        
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f + (Mosaic->lineThickness)), Mosaic->gridColor);
    }
}

void DrawGrid() {
    
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);

        vec2 scale = V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness);
        if (y == 0 || y == Mosaic->gridHeight) {
            scale = V2(Mosaic->gridSize.x * 0.5f + Mosaic->lineThickness, Mosaic->lineThickness);
        }
        
        DrawRect(rowLineCenter, scale, Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);

        vec2 scale = V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f);
        if (x == 0 || x == Mosaic->gridWidth) {
            scale = V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f + Mosaic->lineThickness);
        }
        
        DrawRect(colLineCenter, scale, Mosaic->gridColor);
    }
}

MTile*GetHoveredTile() {
    Camera *cam = &Game->camera;
    
    vec2 mousePos = Input->mousePosNormSigned;
    mousePos.x *= cam->width * 0.5f;
    mousePos.y *= cam->height * 0.5f;

    real32 xDistFromOrig = mousePos.x - Mosaic->gridOrigin.x;
    real32 yDistFromOrig = Mosaic->gridOrigin.y - mousePos.y;

    if (xDistFromOrig < 0 || yDistFromOrig < 0) { return NULL; }

    int32 xCoord = xDistFromOrig / Mosaic->tileSize;
    int32 yCoord = yDistFromOrig / Mosaic->tileSize;

    if (xCoord >= Mosaic->gridWidth || yCoord >= Mosaic->gridHeight) {
        return NULL;
    }

    int32 index = (yCoord * Mosaic->gridWidth) + xCoord;
    return &Mosaic->tiles[index];
}

inline MTile* GetTile(int32 x, int32 y) {
    if (x < 0 || x >= Mosaic->gridWidth) {
        return NULL;
    }
    if (y < 0 || y >= Mosaic->gridHeight) {
        return NULL;
    }
    
    // TODO: clamp these in case they're out of bounds, so we don't crash
    int32 index = (y * Mosaic->gridWidth) + x;
    return &Mosaic->tiles[index];
}

inline int32 GetTileIndex(int32 x, int32 y) {
    if (x < 0 || x >= Mosaic->gridWidth) {
        return -1;
    }
    if (y < 0 || y >= Mosaic->gridHeight) {
        return -1;
    }
    
    int32 index = (y * Mosaic->gridWidth) + x;
    return index;
}

inline int32 GetTileIndex(vec2 pos) {
    return GetTileIndex(pos.x, pos.y);
}

inline int32 GetTileIndex(vec2i pos) {
    return GetTileIndex(pos.x, pos.y);
}

inline MTile*GetTile(vec2i pos) {
    return GetTile(pos.x, pos.y);
}

inline MTile*GetTile(vec2 pos) {
    return GetTile(pos.x, pos.y);
}

void GetTileNeighbors(MTile *tile, MTile **neighbors) {
    vec2i pos = tile->position;

    int32 neighborCount = 0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (x == 0 && y == 0) { continue; }
            
            MTile *neighbor = GetTile(pos.x + x, pos.y + y);

            neighbors[neighborCount++] = neighbor;
        }
    }
}

    
inline void GetTileBlock(int32 x, int32 y, int32 width, int32 height, MTile**tiles, int32 *tilesRetrieved) {
    for (int y_ = y; y_ < height; y_++) {
        for (int x_ = x; x_ < width; x_++) {
            MTile *t = GetTile(x_, y_);

            if (t) {
                tiles[*tilesRetrieved] = t;
                *tilesRetrieved += 1;
            }
        }
    }
}

void SetBlockColor(int32 x, int32 y, int32 width, int32 height, vec4 color) {
    vec2i bottomRight = V2i(x + width, y + height);
    
    for (int y_ = y; y_ < bottomRight.y; y_++) {
        if (y_ < 0 || y_ >= Mosaic->gridHeight) {
            continue;
        }
        
        for (int x_ = x; x_ < bottomRight.x; x_++) {
            if (x_ < 0 || x_ >= Mosaic->gridWidth) {
                continue;
            }
            
            MTile *t = GetTile(x_, y_);
            if (t) {
                t->color = color;
            }
        }
    }
}

void SetBlockColor(vec2 pos, int32 width, int32 height, vec4 color) {
    vec2i posInt = V2i(floorf(pos.x), floorf(pos.y));
    SetBlockColor(posInt.x, posInt.y, width, height, color);
}

void SetBlockColor(vec2i pos, int32 width, int32 height, vec4 color) {
    SetBlockColor(pos.x, pos.y, width, height, color);
}
    
// @BUG: broken
void GetTilesInLine(int32 x0, int32 y0, int32 x1, int32 y1) {
    int32 y = y0;
    r32 error = 0;

    real32 leftX = x0;
    real32 rightX = x1;

    // @TODO: handle all cooridinate cases for drawing top to bottom, bottom to top, etc

    real32 deltaX = GetTileCenter(x1 - x0);
    real32 deltaY = GetTileCenter(y1 - y0);

    real32 deltaError = Abs(deltaY / deltaX);

    for (int32 x = leftX; x < rightX; x++) {
        SetTileColor(x, y, 1, 1, 1);

        error += deltaError;
        if (error >= 0.5f) {
            y += Sign(y);
            error -= 1;
        }
    }
}

void ClearTiles(vec4 color) {
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            MTile*tile = GetTile(x, y);
            tile->color = color;
        }
    }
}

void ClearTiles(real32 r, real32 g, real32 b) {
    ClearTiles(RGB(r, g, b));
}

inline void SetTileColor(int32 x, int32 y, vec4 color) {
    MTile*t = GetTile(x, y);
    if (t) {
        t->color = color;
    }
}

inline void SetTileColor(int32 x, int32 y, real32 r, real32 g, real32 b) {
    MTile*t = GetTile(x, y);
    if (t) {
        t->color = RGB(r, g, b);
    }
}

inline void SetTileColor(vec2 position, real32 r, real32 g, real32 b) {
    MTile*t = GetTile(position);
    if (t) {
        t->color = RGB(r, g, b);
    }
}

inline void SetTileColor(vec2 position, vec4 color) {
    MTile*t = GetTile(position);
    if (t) {
        t->color = color;
    }
}

inline void SetTileColorB(vec2 position, vec4 color) {
    MTile*t = GetTile(position);
    if (t) {
        t->colorB = color;
    }
}

inline void SetTileScale(vec2 position, real32 scale) {
    MTile*t = GetTile(position);
    if (t) {
        t->scale = scale;
    }
}

inline void SetTileRotation(vec2 position, real32 rotation) {
    MTile*t = GetTile(position);
    if (t) {
        t->rotation = rotation;
    }
}

inline void SetTileSprite(vec2 position, Sprite *sprite) {
    MTile*t = GetTile(position);
    if (t) {
        t->sprite = sprite;
    }
}

inline void SetTileSprite(vec2 position, int32 index) {
    MTile*t = GetTile(position);
    if (t) {
        t->sprite = &GM.bokehMasks[index];
    }
}

inline void SetTileDepth(vec2 position, int32 depth) {
    MTile*t = GetTile(position);
    if (t) {
        t->depth = depth;
    }
}

inline void DrawSprite(vec2 position, Sprite *sprite) {
    for (int y = 0; y < sprite->height; y++) {
        for (int x = 0; x < sprite->width; x++) {
            vec2 pos = position + V2(x, y);

            MTile*t = GetTile(pos);

            int32 pixel = (x * 4) + (y * sprite->width * 4);

            if (t == NULL) { continue; }

            // We don't support alpha blending, but you can use an alpha
            // of 0 to specify that a tile shouldn't be colored. 
            if (sprite->data[pixel + 3] == 0) {
                continue;
            }

            t->color.r = sprite->data[pixel] / 255.0f;
            t->color.g = sprite->data[pixel + 1] / 255.0f;
            t->color.b = sprite->data[pixel + 2] / 255.0f;
            t->color.a = sprite->data[pixel + 3] / 255.0f;
        }
    }
}


vec2i GetMousePosition() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position;
    }

    return V2i(-1, -1);
}

int32 GetMousePositionX() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position.x;
    }

    return -1;
}

int32 GetMousePositionY() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position.y;
    }

    return -1;
}

bool TilePositionsOverlap(vec2i a, vec2i b) {
    return a == b;
}

bool TilePositionsOverlap(int32 ax, int32 ay, int32 bx, int32 by) {
    return TilePositionsOverlap(V2i(ax, ay), V2i(bx, by));
}

bool TilePositionsOverlap(vec2 a, vec2 b) {
    vec2i a_ = V2i(a.x, a.y);
    vec2i b_ = V2i(b.x, b.y);

    return a_ == b_;
}

bool TilePositionsOverlap(real32 ax, real32 ay, real32 bx, real32 by) {
    return TilePositionsOverlap(V2i(ax, ay), V2i(bx, by));
}

real32 GetTileCenter(real32 n) {
    int32 i = (int32)n;
    return i + 0.5f;
}

void DrawTextTop(vec4 color, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 position = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, 0.1f);
    DrawText(&Game->monoFont, position, 0.35f, color, true, str);

    va_end(args);
}

void DrawTextTop(vec4 color, real32 scale, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 position = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, 0.1f);
    DrawText(&Game->monoFont, position, 0.35f * scale, color, true, str);

    va_end(args);
}

void DrawTextTile(vec2 pos, float32 size, vec4 color, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 floorPos = V2(floorf(pos.x), -floorf(pos.y));
    
    vec2 position = Mosaic->gridOrigin + floorPos + V2(0.0f, -1.0f);
    DrawText(&Game->monoFont, position, size, color, false, str);

    va_end(args);
}

void DrawTextTile(vec2 pos, float32 size, vec4 color, bool center, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 floorPos = V2(floorf(pos.x), -floorf(pos.y));
    
    vec2 position = Mosaic->gridOrigin + floorPos + V2(0.0f, -1.0f);

    if (center) {
        // only centering horizontally
        position.x += 0.5f;
    }
    
    DrawText(&Game->monoFont, position, size, color, center, str);

    va_end(args);
}

void PushText(const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    MosaicText *text = &Mosaic->text;

    DrawTextScreen(&Game->monoFont, text->cursor, text->size, text->color, false, str);

    FontTable *font = &Game->monoFont;

    text->cursor.y += font->lineHeight * text->size;
}

void SetTextCursor(real32 x, real32 y) {
    MosaicText *text = &Mosaic->text;
    text->cursor.x = x;
    text->cursor.y = y;
}

void MosaicRender() {
    MTile*tiles = Mosaic->tiles;

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b, 1.0f);

    Mosaic->rectBuffer.count = 0;

    {
        vec2 pos = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, -Mosaic->gridSize.y * 0.5f);
        //DrawRect(pos, Mosaic->gridSize * 0.5f, Mosaic->screenColor);
        
        DrawRect(pos, Mosaic->gridSize * 0.5f, V4(0, 0, 0, 1));
    }

    // @HACK: rendering a background
#if 0
    vec2 worldPos = V2(0);
    real32 scaledSize = 40.0f;
    real32 scaleOffset = 0.0f;
    real32 rot = 0;

    vec4 color = V4(0.0f, 0.1f, 0.25f, 1.0f);
    
    DrawMaskedQuad(worldPos,
                   V2(scaledSize + scaleOffset),
                   // @TODO: allow a lil bit of overlap
                   rot,
                   
                   //0.0f,
                   color,
                   &GM.bokehMasks[0]);
#endif

    MTile *sortedTiles = PushArray(&Game->frameMem, MTile, Mosaic->tileCapacity);

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        sortedTiles[i] = tiles[i];
    }

    QuicksortIterative(sortedTiles, sizeof(MTile), Mosaic->tileCapacity, (SortComparator)&MTile_Comparator, &Game->frameMem);

#if 1
    TexturedTileBuffer *buffer = &Mosaic->tileRenderBuffer;
    buffer->count = 0;
    
    DynamicArray<DrawTileCommand> commands = MakeDynamicArray<DrawTileCommand>(&Game->frameMem, 64);

    DrawTileCommand *command = PushBackPtr(&commands);
    command->depth = sortedTiles[0].depth;
    command->sprite = sortedTiles[0].sprite;

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        MTile* tile = &sortedTiles[i];

        // @BUG: why are the ones on top of grid drawn on top of the others?
        {
            vec2 worldPos = GridPositionToWorldPosition(tile->position);

            TexturedTileRenderData data = {};
            real32 scale = (Mosaic->tileSize * 0.5) * tile->scale;
            
            data.colorA = tile->color;
            data.colorB = tile->colorB;
            data.model = TRS(V3(worldPos, 0),
                             AxisAngle(V3(0, 0, 1), tile->rotation),
                             V3(scale, scale, 0.0f));

            buffer->data[buffer->count++] = data;            
        }

        if (tile->depth > command->depth) {
            command = PushBackPtr(&commands);

            *command = {};
            command->depth = tile->depth;
            command->sprite = tile->sprite;
            command->startIndex = i;
        }
        else if (tile->sprite > command->sprite) {
            command = PushBackPtr(&commands);
            
            *command = {};
            command->depth = tile->depth;
            command->sprite = tile->sprite;
            command->startIndex = i;
        }

        command->count++;
    }

    // @TODO: actually instance these

    for (int i = 0; i < commands.count; i++) {
        DrawTileCommand *command = &commands[i];

        if (command->sprite == NULL) { continue; }

#if 0 // non-instanced
        For (j, command->count) {
            //MTile* tile = &sortedTiles[command->startIndex + j];
            DrawTile(tile);
        }
#else 
        RenderTexturedTileBuffer(&Mosaic->tileRenderBuffer, command->startIndex, command->count, command->sprite);
#endif
    }
#else
    
    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        MTile* tile = &sortedTiles[i];

        DrawTile(tile);
    }
#endif

#if 0
    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        MTile*tile = &tiles[i];

#if PAINT_3
        DrawTile(tile);
#else
        DrawTile(tile->position, tile->color);
#endif
    }
#endif

    if (Mosaic->drawGrid) {
        DrawGrid();        
    }
    else if (Mosaic->drawBorder) {
        DrawBorder();
    }
}

// @NOTE: this is here so code can be inserted into MosaicUpdate in any order you want without
// messing up the internal state of the engine.
// Feel free to ignore this if you know what you're doing.
void MosaicUpdateInternal() {
    Tiles = Mosaic->tiles;
    
    Mosaic->hoveredTilePrev = Mosaic->hoveredTile;
    Mosaic->hoveredTile= GetHoveredTile();
    ClearTiles(V4(0));
}

// This function gets called by our game engine every frame.
void MyGameUpdate() {
    MosaicUpdateInternal();
    MyMosaicUpdate();
    MosaicRender();

    // @TODO: get DrawSprite to work if we pass in a color
}


SoundHandle PlaySound(SoundClip clip, real32 volume, bool loop) {
    return PlaySound(&Game->audioPlayer, clip, volume, loop);
}

