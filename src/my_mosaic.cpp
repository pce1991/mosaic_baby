
#include "entity.h"

#define STAINED_GLASS 0
#define PAINT_1 0
#define PAINT_2 1

enum GameState {
                GameState_Init,
                GameState_Running,
};

typedef void GameInitFunc();
typedef void GameUpdateFunc();


struct GameData {
    GameInitFunc *init;
    GameInitFunc *update;
};

#define FOREACH_GAME(OP, ...)                   \
    OP(EmptyGame, __VA_ARGS__)                  \
    OP(MowTheLawnSimple, __VA_ARGS__)                 \
    OP(MowTheLawn, __VA_ARGS__)                 \
    OP(ChaseColor, __VA_ARGS__)                 \
    OP(HerdColor, __VA_ARGS__)                 \


// @NOTE: a game's struct must match the name in this list exactly

#define MAKE_GAME_ID(name, _) GameID_##name,
#define MAKE_GAME_ID_STRING(name, _) "GameID_" #name,
#define MAKE_GAME_STRING(name, _) ""#name,

#define MAKE_INIT_FUNC(name, _) &name##Init,
#define MAKE_UPDATE_FUNC(name, _) &name##Update,

#define GET_GAME_SIZE(name, _) sizeof(name##Data),

enum GameID {
             FOREACH_GAME(MAKE_GAME_ID, args)
             GameID_Count,
};

const char *GameIDNameTable[] = {
                               FOREACH_GAME(MAKE_GAME_ID_STRING, args)
};

const char *GameNameTable[] = {
                               FOREACH_GAME(MAKE_GAME_STRING, args)
};

// @TODO: make a more readable name with spacing

void SetActiveGame(GameID gameID);
void LoadSpriteFromFile(char *path, Sprite *sprite, MemoryArena *arena);

enum GameMode {
    GameMode_MainMenu,
    GameMode_Playing,
};

struct GameMem {
    MemoryArena arena;
    MemoryArena frameMem;

    MemoryArena gameArena;

    EntityManager entityManager;

    GameMode gameMode;

    struct {
        GameID selectedID;
    } mainMenu;

    GameID gameID;
    GameState gameState;
    GameData *gameData;

    Sprite heroSprite;

    DynamicArray<Sprite> bokehMasks;
};

GameMem GM = {};

void MDrawSprite(vec2 pos, Sprite *sprite);
void MDrawCollider_AABB(vec2 position, vec2 min, vec2 max, vec4 color);
void MDrawCollider_Rect(vec2 position, Rect rect, vec4 color);


enum Direction {
                East,
                North,
                West,
                South,
                Direction_Count,
};

Direction VectorToDirection(vec2 v) {
    if (v.x > 0) {
        return East;
    }
    if (v.x < 0) {
        return West;
    }
    if (v.y < 0) {
        return North;
    }
    if (v.y > 0) {
        return South;
    }

    assert(false);
}

int32 TileWidth = 16;
int32 TileHeight = 16;

int32 GridWidth = 10;
int32 GridHeight = 10;

vec2 TilePositionToPixel(int32 x, int32 y) {
    return V2(x * TileWidth, y * TileHeight);
}



#include "games/chase_color.cpp"
#include "games/herd_color.cpp"
#include "games/mow_the_lawn.cpp"


struct EmptyGameData {};

void EmptyGameInit() {
}

void EmptyGameUpdate() {
}




int64 GameStructSizeTable[] = {
                               FOREACH_GAME(GET_GAME_SIZE, args)
};

GameInitFunc *InitFuncTable[] = {
                                 FOREACH_GAME(MAKE_INIT_FUNC, args)
};

GameUpdateFunc *UpdateFuncTable[] = {
                                 FOREACH_GAME(MAKE_UPDATE_FUNC, args)
};

// @TODO: just give an init&update function to each game's struct



void MDrawSprite(vec2 pos, Sprite *sprite) {
    int32 index = 0;
    vec4 color = {};
    
    vec4 *colorBuffer = (vec4*)sprite->data;
                
    for (int y = 0; y < sprite->height; y++) {
        for (int x = 0; x < sprite->width; x++) {
            color = colorBuffer[index++];
            
            if (color.a == 0) {
                continue;
            }
            
            SetTileColor(pos.x + x, pos.y + y, color);
        }
    }
}

void AllocateSprite(MemoryArena *arena, Sprite *sprite, int32 width, int32 height) {
    sprite->width = width;
    sprite->height = height;
    sprite->size = sprite->width * sprite->height * sizeof(vec4);

    sprite->data = (uint8 *)PushSize(arena, sprite->size);
}

// @BUG: this loads data in a different packing order than sprites to upload to GPU
void LoadSpriteFromFile(char *path, Sprite *sprite, MemoryArena *arena) {
    int32 x, y, components;
    uint8 *data = stbi_load(path, &x, &y, &components, 0);
    // assuming 4 color channels

    if (data == NULL) {
        assert(false);
    }

    AllocateSprite(arena, sprite, x, y);
    
    int32 colorIndex = 0;
    int32 dataLength = x * y * components;

    // @TODO: treat *data as a buffer of vec4s
    
    vec4 *colorBuffer = (vec4*)sprite->data;
                
    vec4 color;
    switch (components) {
        default : {assert(false); } break;
        case 4 : {
            for (int i = 0; i < dataLength; i += 4) {
                colorBuffer[colorIndex++] = V4(data[i] / 255.0f,
                                               data[i + 1] / 255.0f,
                                               data[i + 2] / 255.0f,
                                               data[i + 3] / 255.0f);
            }
        }
    }

    free(data);
}


    
// struct CollisionEvent {
//     Entity *a;
//     Entity *b;
// };

// struct Physics {
//     DynamicArray<CollisionEvent> events;
// };


void SetActiveGame(GameID gameID) {
    GM.gameMode = GameMode_Playing;
    ClearMemoryArena(&GM.gameArena);

    GM.gameData = (GameData *)PushSize(&GM.gameArena, GameStructSizeTable[gameID]);
    
    GM.gameData->init = InitFuncTable[gameID];
    GM.gameData->update = UpdateFuncTable[gameID];
}

void MyMosaicInit() {
    AllocateMemoryArena(&GM.arena, Megabytes(64));
    AllocateMemoryArena(&GM.frameMem, Megabytes(16));
    AllocateMemoryArena(&GM.gameArena, Megabytes(16));

    // THIS CANNOT BE USED ON THE GPU BECAUSE OF HOW LoadSpriteFromFile works
    LoadSpriteFromFile("data/sprites/hero_down.png", &GM.heroSprite, &GM.arena);

    {
        GM.bokehMasks = MakeDynamicArray<Sprite>(&GM.arena, 16);

#if STAINED_GLASS
        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/bokeh/bokeh_paint1.png");
        }

        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/bokeh/bokeh_paint2.png");
        }
#elif PAINT_1
        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/paint/dots.png");
        }
#elif PAINT_2
        // {
        //     Sprite *s = PushBackPtr(&GM.bokehMasks);
        //     LoadSprite(s, "data/paint/waves.png");
        // }

        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/paint/dots.png");
        }
        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/paint/smear2.png");
        }
        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/paint/smear.png");
        }
#endif
        #if 0
        {
            Sprite *s = PushBackPtr(&GM.bokehMasks);
            LoadSprite(s, "data/sprites/brush_white.png");
        }
#endif
    }

    GM.gameMode = GameMode_MainMenu;
    GM.mainMenu.selectedID = (GameID)1;
}

void MDrawCollider_AABB(vec2 position, vec2 min, vec2 max, vec4 color) {
    vec2 dim = max - min;

    for (int y = 0; y < dim.y; y++) {
        for (int x = 0; x < dim.x; x++) {
            if ((y > 0 && y < dim.y - 1) && (x > 0 && x < dim.x - 1)) {
                continue;
            }
            
            SetTileColor(position + V2(x, y) + min, color);
        }
    }
}

void MDrawCollider_Rect(vec2 position, Rect rect, vec4 color) {
    MDrawCollider_AABB(position, rect.min, rect.max, color);
}

#if 0
void DetectCollisions() {
    EntityManager *em = &GM.entityManager;
    
    Player *player = &GM.player;
    Rock *rock = &GM.rock;

    vec2 playerCenterWorld = player->position + player->center;
    
    vec2 playerMinWorld = player->position + player->min;
    vec2 playerMaxWorld = player->position + player->max;

    for (int i = 0; i < em->rocks.count; i++) {
        Rock *rock = &em->rocks[i];

        vec2 rockMinWorld = rock->position + rock->min;
        vec2 rockMaxWorld = rock->position + rock->max;

        vec2 dir = V2(0);
        if (TestCircleAABB(playerCenterWorld, player->radius, rockMinWorld, rockMaxWorld, &dir)) {
            player->position = player->position + dir;
        }
    }

    for (int i = 0; i < em->trees.count; i++) {
        Tree *tree = &em->trees[i];

        vec2 treeMinWorld = tree->position + tree->min;
        vec2 treeMaxWorld = tree->position + tree->max;

        vec2 dir = V2(0);
        if (TestCircleAABB(playerCenterWorld, player->radius, treeMinWorld, treeMaxWorld, &dir)) {
            player->position = player->position + dir;
        }
    }
}
#endif

void MainMenuUpdate() {
    // @HEAP
    SetMosaicGridSize(5, 5);

    // Important to know that these are signed enums because of the - 1!
    if (InputPressed(Keyboard, Input_UpArrow)) {
        GM.mainMenu.selectedID = (GameID)(GM.mainMenu.selectedID - 1);
    }
    else if (InputPressed(Keyboard, Input_DownArrow)) {
        GM.mainMenu.selectedID = (GameID)(GM.mainMenu.selectedID + 1);
    }

    if (GM.mainMenu.selectedID >= GameID_Count) {
        GM.mainMenu.selectedID = (GameID)1; // because 1 == GameID_Empty
    }
    else if (GM.mainMenu.selectedID <= 0) {
        GM.mainMenu.selectedID = (GameID)(GameID_Count - 1);
    }

    SetMosaicScreenColor(V4(0, 0, 0, 1));

    // @TODO: array of tiles which change color all the time at different rates

    // SetTileColor(2, 2, 1, 0, 0);
    // SetTileColor(2, 4, 1, 0, 0);

    GameID prevID = (GameID)(GM.mainMenu.selectedID - 1);
    if (prevID <= 0) {
        prevID = (GameID)(GameID_Count - 1);
    }
    GameID nextID = (GameID)(GM.mainMenu.selectedID + 1);

    if (nextID >= GameID_Count) {
        nextID = (GameID)(1);
    }

    DrawTextTile(V2(2, 0), 0.2f, V4(1.0f, 1.0f, 1.0f, 0.5f), true, "%d) %s", (int32)prevID, GameNameTable[prevID]);
    
    DrawTextTile(V2(2, 1), 0.3f, V4(1.0f, 1.0f, 1.0f, 1.0f), true, "%d) %s", (int32)GM.mainMenu.selectedID, GameNameTable[GM.mainMenu.selectedID]);

    DrawTextTile(V2(2, 2), 0.2f, V4(1.0f, 1.0f, 1.0f, 0.5f), true, "%d) %s", (int32)nextID, GameNameTable[nextID]);

    // @BUG: this should be centered but it isn't
    DrawTextTile(V2(2, 4), 0.3f, V4(1.0f, 1.0f, 1.0f, 1.0f), true, "Press Space to Start");

    if (InputPressed(Keyboard, Input_Space) ||
        InputPressed(Keyboard, Input_Return)
        ) {
    SetActiveGame(GM.mainMenu.selectedID);
    }
}


void MyMosaicUpdate() {
    MDrawSprite(V2(50, 50), &GM.heroSprite);

    // @TODO: do this with function pointers

    if (InputPressed(Keyboard, Input_Backspace)) {
        GM.gameMode = GameMode_MainMenu;
    }

    switch (GM.gameMode) {
        case GameMode_MainMenu : {
            MainMenuUpdate();
        } break;
            
        case GameMode_Playing : {
            GameData *gameData = GM.gameData;

            if (GM.gameState == GameState_Init) {
                gameData->init();
                GM.gameState = GameState_Running;
            }

            gameData->update();
        } break;
    }
}

