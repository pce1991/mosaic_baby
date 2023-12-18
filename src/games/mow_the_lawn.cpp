
/* @TODO: 
   Gonna want more sophisticated audio mixing: handles, fades
   Would be nice to auto genearte rotated sprites
   Entities would help ease some of the boiler plate

   @TODO: get grass variations
   @TODO: get xbox gamepad support and use zach's controller!
 */

struct MTGGrass {
    vec2 position;
    bool cut;

    int8 spriteIndex;

    Rect rect;
};

struct MTGMower {
    vec2 position;
    vec2 velocity;

    Direction facingDir;

    Rect rect;
};

struct MowTheLawnData : GameData {
    DynamicArray<Sprite> uncutGrassSprites;
    DynamicArray<Sprite> cutGrassSprites;
    Sprite mowerSprites[Direction_Count];

    SoundClip mowerSound;

    DynamicArray<MTGGrass> grass;
    MTGMower mower;
};


void MowTheLawnInit() {
    MowTheLawnData *Data = (MowTheLawnData*)GM.gameData;

    TileWidth = 8;
    TileHeight = 8;

    GridWidth = 20;
    GridHeight = 20;

    // @DESIGN: grass should probably be smaller than the mower

    SetMosaicGridSize(TileWidth * GridWidth,
                      TileHeight * GridHeight);

    Data->uncutGrassSprites = MakeDynamicArray<Sprite>(&GM.gameArena, 4);
    Data->cutGrassSprites = MakeDynamicArray<Sprite>(&GM.gameArena, 4);

    LoadSoundClip("data/sfx/lawn_mower.wav", &Data->mowerSound);

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut1.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut2.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut3.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut4.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->cutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_cut1.png", sprite, &GM.gameArena);
    }

    {
        LoadSpriteFromFile("data/sprites/lawn_mower.png", &Data->mowerSprites[North], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_south.png", &Data->mowerSprites[South], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_east.png", &Data->mowerSprites[East], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_west.png", &Data->mowerSprites[West], &GM.gameArena);
    }

    {
        MTGMower *mower = &Data->mower;
        *mower = {};
        mower->position = TilePositionToPixel(5, 5);
        mower->rect.min = V2(2, 2);
        mower->rect.max = V2(14, 14);
    }

    
    Data->grass = MakeDynamicArray<MTGGrass>(&GM.gameArena, GridWidth * GridHeight);

    {
        For (y, GridHeight) {
            For (x, GridWidth) {
                vec2 pos = TilePositionToPixel(x, y);

                MTGGrass g = {};
                g.position = pos;
                g.rect.min = V2(2, 2);
                g.rect.max = V2(6, 6);

                // @BUG: why is RandiRange not that random???
                g.spriteIndex = RandiRange(0, Data->uncutGrassSprites.count);

                Print("(%d %d) index = %d", x, y, g.spriteIndex);
                
                //g.spriteIndex = RandfRange(0, Data->uncutGrassSprites.count);

                PushBack(&Data->grass, g);
            }
        }
    }

    PlaySound(&Game->audioPlayer, Data->mowerSound, 1.0f, true);
}

void MTGMower_Update(MowTheLawnData *Data, MTGMower *mower) {
    float32 maxSpeed = 40;

    vec2 dir = V2(0);
    if (InputHeld(Keyboard, Input_LeftArrow) ||
        InputHeld(Gamepad, Input_DirLeft)) {
        dir.x = -1;
        mower->facingDir = West;
    }
    else if (InputHeld(Keyboard, Input_RightArrow) ||
             InputHeld(Gamepad, Input_DirRight)) {
        dir.x = 1;
        mower->facingDir = East;
    }
    else if (InputHeld(Keyboard, Input_UpArrow) ||
             InputHeld(Gamepad, Input_DirUp)) {
        dir.y = -1;
        mower->facingDir = North;
    }
    else if (InputHeld(Keyboard, Input_DownArrow) ||
             InputHeld(Gamepad, Input_DirDown)) {
        dir.y = 1;
        mower->facingDir = South;
    }

    float32 damping = 3.5;
    mower->velocity = mower->velocity + (-mower->velocity * damping * DeltaTime);

    float32 force = 67;
    mower->velocity = mower->velocity + dir * force * DeltaTime;

    if (Length(mower->velocity) > maxSpeed) {
        mower->velocity = Normalize(mower->velocity) * maxSpeed;
    }

    vec2 prevPos = mower->position;

    mower->position = mower->position + mower->velocity * DeltaTime;

    Rect worldRect = GlobalRect(mower->position, mower->rect);
    if (!TestPointAABB(worldRect.min, V2(0, 0), V2(TileWidth * GridWidth, TileHeight * GridHeight)) ||
        !TestPointAABB(worldRect.max, V2(0, 0), V2(TileWidth * GridWidth, TileHeight * GridHeight))) {
        mower->position = prevPos;
        mower->velocity = V2(0);
    }

    For (i, Data->grass.count) {
        MTGGrass *grass = &Data->grass[i];

        vec2 dir = V2(0);
        if (TestRects(mower->rect, grass->rect, mower->position, grass->position, &dir)) {
            grass->cut = true;
        }
    }
}

void MowTheLawnUpdate() {
    MowTheLawnData *Data = (MowTheLawnData*)GM.gameData;

    MTGMower_Update(Data, &Data->mower);

    // @TODO: check for collision with grass

    For (i, Data->grass.count) {
        MTGGrass *grass = &Data->grass[i];
        Sprite *sprite = &Data->uncutGrassSprites[grass->spriteIndex];

        if (grass->cut) {
            sprite = &Data->cutGrassSprites[0];
        }
        
        MDrawSprite(grass->position, sprite);
    }

    MTGMower *mower = &Data->mower;
    
    MDrawSprite(mower->position, &Data->mowerSprites[mower->facingDir]);
    //MDrawCollider_Rect(mower->position, mower->rect, WHITE);;
}



struct MowTheLawnSimpleData : GameData {
    DynamicArray<Sprite> uncutGrassSprites;
    DynamicArray<Sprite> cutGrassSprites;
    Sprite mowerSprites[Direction_Count];

    SoundClip mowerSound;

    DynamicArray<MTGGrass> grass;
    MTGMower mower;
};

void MowTheLawnSimpleInit() {
    MowTheLawnData *Data = (MowTheLawnData*)GM.gameData;

    TileWidth = 8;
    TileHeight = 8;

    GridWidth = 12;
    GridHeight = 2;

    // @DESIGN: grass should probably be smaller than the mower

    SetMosaicGridSize(TileWidth * GridWidth,
                      TileHeight * GridHeight);

    Data->uncutGrassSprites = MakeDynamicArray<Sprite>(&GM.gameArena, 4);
    Data->cutGrassSprites = MakeDynamicArray<Sprite>(&GM.gameArena, 4);

    LoadSoundClip("data/sfx/lawn_mower.wav", &Data->mowerSound);

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut1.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut2.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut3.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->uncutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_uncut4.png", sprite, &GM.gameArena);
    }

    {
        Sprite *sprite = PushBackPtr(&Data->cutGrassSprites);
        LoadSpriteFromFile("data/sprites/grass_cut1.png", sprite, &GM.gameArena);
    }

    {
        LoadSpriteFromFile("data/sprites/lawn_mower.png", &Data->mowerSprites[North], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_south.png", &Data->mowerSprites[South], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_east.png", &Data->mowerSprites[East], &GM.gameArena);
        LoadSpriteFromFile("data/sprites/lawn_mower_west.png", &Data->mowerSprites[West], &GM.gameArena);
    }

    {
        MTGMower *mower = &Data->mower;
        *mower = {};
        mower->facingDir = East;
        mower->position = V2(0, 0);
        mower->rect.min = V2(2, 2);
        mower->rect.max = V2(14, 14);
    }

    
    Data->grass = MakeDynamicArray<MTGGrass>(&GM.gameArena, GridWidth * GridHeight);

    {
        For (y, GridHeight) {
            For (x, GridWidth) {
                vec2 pos = TilePositionToPixel(x, y);

                MTGGrass g = {};
                g.position = pos;
                g.rect.min = V2(2, 2);
                g.rect.max = V2(6, 6);

                // @BUG: why is RandiRange not that random???
                g.spriteIndex = RandiRange(0, Data->uncutGrassSprites.count);

                Print("(%d %d) index = %d", x, y, g.spriteIndex);
                
                //g.spriteIndex = RandfRange(0, Data->uncutGrassSprites.count);

                PushBack(&Data->grass, g);
            }
        }
    }

    PlaySound(&Game->audioPlayer, Data->mowerSound, 1.0f, true);
}

void MTGMowerSimple_Update(MowTheLawnSimpleData *Data, MTGMower *mower) {
    float32 maxSpeed = 40;

    vec2 dir = V2(0);
    if (InputHeld(Keyboard, Input_Space) ||
        InputHeld(Gamepad, Input_FaceA) ||
        InputHeld(Gamepad, Input_FaceB)
        ) {
        dir.x = 1;
        mower->facingDir = East;
    }

    float32 damping = 3.5;
    mower->velocity = mower->velocity + (-mower->velocity * damping * DeltaTime);

    float32 force = 67;
    mower->velocity = mower->velocity + dir * force * DeltaTime;

    if (Length(mower->velocity) > maxSpeed) {
        mower->velocity = Normalize(mower->velocity) * maxSpeed;
    }

    vec2 prevPos = mower->position;

    mower->position = mower->position + mower->velocity * DeltaTime;

    // @TODO: move all the grass instead of the mower? 

    Rect worldRect = GlobalRect(mower->position, mower->rect);

    //if (!TestPointAABB(worldRect.min, V2(0, 0), V2(TileWidth * GridWidth, TileHeight * GridHeight))) {
    if (worldRect.min.x >= TileWidth * GridWidth) {
        mower->position.x = -16;

        For (i, Data->grass.count) {
            MTGGrass *g = &Data->grass[i];
            g->cut = false;
        }
    }

        
    // if (!TestPointAABB(worldRect.min, V2(0, 0), V2(TileWidth * GridWidth, TileHeight * GridHeight)) ||
    //     !TestPointAABB(worldRect.max, V2(0, 0), V2(TileWidth * GridWidth, TileHeight * GridHeight))) {
    //     mower->position = prevPos;
    //     mower->velocity = V2(0);
    // }

    For (i, Data->grass.count) {
        MTGGrass *grass = &Data->grass[i];

        vec2 dir = V2(0);
        if (TestRects(mower->rect, grass->rect, mower->position, grass->position, &dir)) {
            grass->cut = true;
        }
    }
}

void MowTheLawnSimpleUpdate() {
    MowTheLawnSimpleData *Data = (MowTheLawnSimpleData *)GM.gameData;

    MTGMowerSimple_Update(Data, &Data->mower);


    // idea is to set some roation for everything rather than having to do it per MDrawSprite
    For (y, Mosaic->gridHeight) {
        For (x, Mosaic->gridWidth) {
            real32 signedNoise = Sum1f(&Perlin1f, ((Time * 2.0 + ((x + y * 19) * 0.5f)) * 0.2f), 1.0f, 1.0f);
            real32 normNoise = (1 + signedNoise) * 0.5f;
            
            vec2 pos = V2(x, y);

            SetTileColorB(pos, V4(0, 0, 0, 1));
            
            SetTileRotation(pos, (x + (y * 10)) * _PI * 0.08f);
            SetTileScale(pos, 1.5 + (normNoise * 0.5f));
            SetTileDepth(pos, 0);
        }
    }

    For (i, Data->grass.count) {
        MTGGrass *grass = &Data->grass[i];
        Sprite *sprite = &Data->uncutGrassSprites[grass->spriteIndex];

        if (grass->cut) {
            sprite = &Data->cutGrassSprites[0];
        }
        
        MDrawSprite(grass->position, sprite, 1);
    }

    MTGMower *mower = &Data->mower;
    
    MDrawSprite(mower->position, &Data->mowerSprites[mower->facingDir], 2);
    //MDrawCollider_Rect(mower->position, mower->rect, WHITE);;
}
