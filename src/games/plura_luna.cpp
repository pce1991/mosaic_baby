
enum PLSceneID {
     PLSceneID_None,
     PLSceneID_MoonPelt,
     PLSceneID_Moonlight,
     PLSceneID_Pond,
     PLSceneID_Eclipse,
     PLSceneID_Campfire,
     PLSceneID_Count,
};

struct PLScene {
    PLSceneID id;
    real32 startTime;
    bool firstFrame;

    int32 gridWidth;
    int32 gridHeight;

    bool inited;
    bool active;
    void *data;
};

struct PluraLunaData : GameData {
    bool startNextScene;
    
    PLSceneID currScene;
    PLSceneID nextScene;
    PLSceneID prevScene;

    PLScene scenes[PLSceneID_Count];
};


struct MoonCrater {
    vec2i offsetPos;
    real32 timeHit;
    real32 radius;
    bool hitEarth;
};

struct MoonPelt {
    vec2 moonPos;
    real32 moonDepth;
    int32 moonDir;

    real32 hoveredTileRadius;
    real32 hoveredTileRadiusDir;
    DynamicArray<MoonCrater> craters;

    SoundClip backingClip;
    DynamicArray<SoundClip> meteorSounds;
    int32 soundIndex;
    real32 soundTime;
};


struct EarthCrack {
    vec2 earthPos;

    bool shotReleased;
    bool shotStarted;
    real32 clickStartTime;
    vec2i clickStartPos;
    vec2i clickFinalPos;

    real32 shotForce;
    real32 lastSampleTime;
    DynamicArray<vec2i> pathPoints;

    vec2 moonStartPos;

    vec2 meteorVel;
    vec2 meteorPos;

    real32 timeCollided;
    bool collided;
    vec2i collidePoint;
    bool moonSpawned;
    vec2 moonVel;
    vec2 moonPos;
};

struct Wave {
    MTile *tile;
    vec2i dir;
    real32 timeCrest;

    real32 time;
    vec2i orig;
    real32 radius;

    real32 strength;
    real32 height;
};


struct BoatTides {
    real32 simulationTime;
    
    vec2 moonPos;

    vec2 boatPos;
    vec2 boatFacingDir;
    vec2 boatVel;
    vec2i rowDirection;

    int32 moonDir;

    DynamicArray<Wave> waves;

    DynamicArray<Wave> waves2;
    
    real32 waveRate;
    real32 lastWaveTime;

    real32 waveUpdateTime;

    vec4 oceanColor;
    vec4 waveColor;
};

struct Pond {
    vec2 moonPos;

    vec2i mousePosi;
    vec2i mousePosiPrev;

    vec4 pondColor;

    DynamicArray<Wave> waves;
    DynamicArray<vec3> tileNormals;

    SoundClip song;
};

struct Eclipse {
    vec2 sunPos;
    vec2 moonPos;

    vec2 moonVel;
    bool grab;
    bool grabbed;
    vec2i mouseStartPosi;
    vec2i mousePosi;
    vec2i mousePosiPrev;

    int32 soundIndex;
    SoundClip *clips;
    real32 lastPlayTime;
};

struct Cloud {
    vec2 pos;
    vec2 vel;
    bool dispersed;
};

struct Critter {
    vec2i pos;
    real32 timeMoved;
    vec4 color;
};

struct Moonlight {
    vec2 moonPos;
    vec2 moonVel;

    int32 depthRange;

    DynamicArray<Cloud> clouds;

    bool grab;
    vec2i mouseStartPosi;
    vec2i mousePosi;
    vec2i mousePosiPrev;

    DynamicArray<Critter> critters;

    SoundClip horseSong;
    SoundClip windSong;
};


struct GuitString {
    vec2i pos;
    int32 pluckedDir;
};
    
struct Campfire {
    DynamicArray<GuitString> strings;

    SoundClip song;
    SoundClip *clips[6];
    int32 soundIndex[6];
    int32 soundsPlayed;

    bool grab;
    vec2i mouseStartPosi;
    vec2i mousePosi;
    vec2i mousePosiPrev;
};


PluraLunaData *Data = NULL;

void MoonPeltInit(PLScene *scenePtr) {
    scenePtr->data = PushSize(&GM.gameArena, (sizeof(MoonPelt)));

    SetMosaicGridSize(49, 48);
     
    scenePtr->gridWidth = 49;
    scenePtr->gridHeight = 48;

    MoonPelt *moon = (MoonPelt *)scenePtr->data;
    memset(moon, 0, sizeof(MoonPelt));

    moon->moonPos = V2(0, 24);

    moon->moonDir = 1;
    
    LoadSoundClip("data/sleepwalking.wav", &moon->backingClip);

    moon->meteorSounds = MakeDynamicArray<SoundClip>(&GM.gameArena, 20);
    LoadSoundClip("data/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_7.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_8.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_9.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_10.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_11.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_12.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_13.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/meteor_3.wav", PushBackPtr(&moon->meteorSounds));

    moon->craters = MakeDynamicArray<MoonCrater>(&GM.gameArena, 20);
    

    moon->hoveredTileRadiusDir = 1;
    moon->hoveredTileRadius = 0.1f;   
}

void MoonPeltUpdate(PLScene *scenePtr, void *sceneData) {
    Data = (PluraLunaData *)GM.gameData;
    MoonPelt *scene = (MoonPelt *)sceneData;

    if (scenePtr->firstFrame) {
        PlaySound(&Game->audioPlayer, scene->backingClip, false);
    }

    vec2 moonPosPrev = scene->moonPos;
    vec2 *moonPos = &scene->moonPos;
    vec2i moonPosi = V2i(moonPos->x, moonPos->y);

    real32 speed = 5;
    moonPos->x += DeltaTime * speed * scene->moonDir;

    real32 moonFrontRadiusMax = 11;
    real32 moonFrontRadiusMin = 6;

    real32 moonBackRadiusMax = 4;
    real32 moonBackRadiusMin = 1;

    real32 moonRadius = 0;

    vec2 earthPos = V2(24, 24);
    vec2i earthPosi = V2i(24, 24);
    real32 earthRadius = 17;

    real32 moonDistFromEarthCenterPrev = Distance(earthPos, *moonPos);
    real32 moonDistFromEarthCenter = Distance(earthPos, *moonPos);

    // @TODO: need to factor in the min radius when we're putting on the range -1 to 1
    real32 moonPosXNormalized = ((moonPos->x / ((real32)Mosaic->gridWidth)) - 0.5f) * 2;

    // I want this to be the same function
    if (scene->moonDir > 0) {
        moonRadius = moonFrontRadiusMax - (Abs(moonPosXNormalized) * moonFrontRadiusMin);
    }
    else {
        moonRadius = moonBackRadiusMin + (Abs(moonPosXNormalized) * moonBackRadiusMax);
    }

    if (moonPos->x > Mosaic->gridWidth + moonRadius && scene->moonDir > 0) {
        Print("raidus %f", moonRadius);
        scene->moonDir = -1;
    }
    if (moonPos->x < -moonRadius && scene->moonDir < 0) {
        Print("raidus %f", moonRadius);
        scene->moonDir = 1;
    }

    // @TODO: store data that's the size of the moon at max radius.
    // Move that with the moon, and only provide access to the current
    // radius. Then if you click store the time they were hit,
    // What about when you hit the same spot multiple times?
    // Maybe we only need to store the data based on when 
    
    // Change the size of the meteors
    
    if (Mosaic->hoveredTilePrev != NULL) {
        Mosaic->hoveredTilePrev->color = V4(0);
    }

    
    scene->hoveredTileRadius += 2.0f * DeltaTime * scene->hoveredTileRadiusDir;
    if (scene->hoveredTileRadius >= 2.5f) {
        scene->hoveredTileRadius = 2.5f;
        scene->hoveredTileRadiusDir = -1;
    }
    else if (scene->hoveredTileRadius <= 0) {
        scene->hoveredTileRadius = 0.1f;
        scene->hoveredTileRadiusDir = 1;
    }

    real32 hoveredTileRadius = scene->hoveredTileRadius;

    if (Mosaic->hoveredTile != NULL) {
        if (InputPressed(Mouse, Input_MouseLeft)) {
            MoonCrater crater = {};

            r32 distEarth = Distance(Mosaic->hoveredTile->position, earthPosi);

            bool inEarth = false;
            if (distEarth < earthRadius) {
                inEarth = true;
            }

            bool inMoon = false;
            r32 dist = Distance(Mosaic->hoveredTile->position, moonPosi);
            if (dist < moonRadius) {
                inMoon = true;
            }

            // Check if it hits the earth of the moon:
            if (inMoon) {
                crater.offsetPos = Mosaic->hoveredTile->position - moonPosi;    
            }
            else if (inEarth) {
                crater.offsetPos = Mosaic->hoveredTile->position - earthPosi;
                crater.hitEarth = true;
            }
            
            crater.timeHit = Game->time;

            // @TODO: instead od doing this vary the cursor size over time.
            real32 craterRadii[6] = { 0.1f, 0.1f, 0.1f, 1.1f, 1.1f, 2.5f};
            crater.radius = craterRadii[RandiRange(0, 6)];

            crater.radius = hoveredTileRadius;

            SoundClip *clip = NULL;
            clip = &scene->meteorSounds[scene->soundIndex];
            
            PlaySound(&Game->audioPlayer, *clip, false);

            PushBack(&scene->craters, crater);
        }
    }

    if (Time - scene->soundTime > 3.0f) {
        
        scene->soundIndex++;
                    Print("index is %d", scene->soundIndex);
        scene->soundTime = Game->time;
          if (scene->soundIndex > 19) {
              scene->soundIndex = 0;
          }
    }

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        //real32 signedNoise = Sum1f(&Perlin1f, ((Time * 1.0 + (i * 0.5f)) * 0.2f), 1.0f, 1.0f);
        real32 signedNoise = Sum1f(&Perlin1f, ((i * 0.5f)) * 0.3f, 1.0f, 1.0f);
        real32 normNoise = (1 + signedNoise) * 0.5f;
            
        MTile *tile = &Mosaic->tiles[i];

        tile->color = V4(0);

        vec2 pos = V2(tile->position.x, tile->position.y);

        //SetTileScale(pos, 1.0 + (normNoise * 0.9f));
        //SetTileColor(pos, V4(0.05f, 0.05f, 0.05f, 1.0f));

        SetTileColor(pos, Lerp(V4(0.05f, 0.05f, 0.05f, 1.0f),
                               V4(0.15f, 0.15f, 0.15f, 1.0f),
                               normNoise
                               ));
        
        SetTileScale(pos, 2.0f);
        SetTileRotation(pos, (i * 0.1f) + signedNoise);
        SetTileSprite(pos, 1);
        SetTileDepth(pos, 0);
    }

    vec4 oceanColor = V4(0.0f, .2f, 0.9f, 1.0f);    

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            MTile *tile = GetTile(x, y);
            if (tile == NULL) { continue; }

            int32 index = x + (y * Mosaic->gridHeight);

            vec2 pos = V2(tile->position.x, tile->position.y);

            r32 distEarth = Distance(tile->position, earthPosi);

            real32 signedNoise = Sum1f(&Perlin1f, ((Time + (index * 0.5f))) * 0.2f, 1.0f, 1.0f);
            real32 normNoise = (1 + signedNoise) * 0.5f;

            bool inEarth = false;
            if (distEarth < earthRadius) {
                tile->color = oceanColor;
                tile->color.a = 0.6;

                inEarth = true;

                vec2 pos = V2(tile->position.x, tile->position.y);

                SetTileScale(pos, 2.0f);
                //SetTileRotation(pos, Time + (index * 0.1f) + signedNoise);
                //SetTileRotation(pos, sinf(((Time * 0.1f) + (index * 0.1)) * _PI) * _PI);
                SetTileRotation(pos, normNoise * _PI);
                    //;Time + (index * 0.1f) + signedNoise);
            }

            for (int i = 0; i < scene->craters.count; i++) {
                MoonCrater crater = scene->craters[i];

                if (!crater.hitEarth) {
                    continue; 
                }
                
                real32 craterDist = Distance(crater.offsetPos + earthPosi, tile->position);
                real32 craterFromCenter = Distance(crater.offsetPos, V2i(0, 0));

                real32 timeSinceHit = Game->time - crater.timeHit;

                vec4 colors[3] = {V4(1.0f, 1.0f, 1.0f, 1.0f),
                                  oceanColor,
                                  oceanColor,
                };

                real32 timeRanges[2] = {0.6f, 0.8f};

                real32 radiusBloomTime = 0.3f;
                real32 bloomT = Clamp(timeSinceHit / radiusBloomTime, 0.0f, 1.0f);
                real32 radius = (crater.radius * 0.5f) + Lerp(2.0f, 0.0f, bloomT);

                // @TODO: change the radius over time too!

                if (craterDist < radius && craterFromCenter < earthRadius) {
                    if (timeSinceHit <= timeRanges[0]) {
                        real32 dur = timeRanges[0];
                        real32 t = timeSinceHit / dur;
                        tile->color = Lerp(colors[0], colors[1], t);
                    }
                    else if (timeSinceHit <= timeRanges[1]) {
                        real32 dur = timeRanges[1] - timeRanges[0];
                        real32 t = (timeSinceHit - timeRanges[0]) / dur;
                        tile->color = Lerp(colors[1], colors[2], t);
                    }
                    else {
                        tile->color = oceanColor;
                    }

                    tile->depth = 1;
                }
            }
            
            r32 dist = Distance(tile->position, moonPosi);

            bool inMoon = false;

            if (!(scene->moonDir < 0 && inEarth) && dist < moonRadius) {
                tile->color = V4(1);
                tile->depth = 2;
                inMoon = true;

                SetTileRotation(pos, _PI * normNoise);

                SetTileSprite(pos, 0);

                for (int i = 0; i < scene->craters.count; i++) {
                    MoonCrater crater = scene->craters[i];
                    if (crater.hitEarth) { continue; }
                    
                    real32 craterDist = Distance(crater.offsetPos + moonPosi, tile->position);
                    real32 craterFromCenter = Distance(crater.offsetPos, V2i(0, 0));

                    real32 timeSinceHit = Game->time - crater.timeHit;

                    vec4 colors[3] = {V4(1.0f, 0.7f, 0.0f, 1.0f),
                                      V4(0.5f, 0.1f, 0.0f, 1.0f),
                                      V4(0.3f, 0.3f, 0.3f, 1.0f)
                    };

                    real32 timeRanges[2] = {0.6f, 0.8f};

                    real32 radiusBloomTime = 0.3f;
                    real32 bloomT = Clamp(timeSinceHit / radiusBloomTime, 0.0f, 1.0f);
                    real32 radius = crater.radius + Lerp(2.0f, 0.0f, bloomT);

                    // @TODO: change the radius over time too!

                    if (craterDist < radius && craterFromCenter < moonRadius) {
                        if (timeSinceHit <= timeRanges[0]) {
                            real32 dur = timeRanges[0];
                            real32 t = timeSinceHit / dur;
                            tile->color = Lerp(colors[0], colors[1], t);
                        }
                        else if (timeSinceHit <= timeRanges[1]) {
                            real32 dur = timeRanges[1] - timeRanges[0];
                            real32 t = (timeSinceHit - timeRanges[0]) / dur;
                            tile->color = Lerp(colors[1], colors[2], t);
                        }
                        else {
                            tile->color = V4(0.3f, 0.3f, 0.3f, 1.0f);
                        }

                        tile->depth = 2;
                    }
                }
            }
            
            bool inCursor = true;
            if (Mosaic->hoveredTile != NULL) {
                r32 distCursor = Distance(Mosaic->hoveredTile->position, tile->position);
                if (distCursor < hoveredTileRadius) {
                    tile->color = V4(1.0f, 0.24f, 0.0f, 1.0f);
                    inCursor = true;
                }
            }

            // I don't think we need this anymore, we want to render everything
            // if (inMoon || inEarth || inCursor) {
            //     tile->active = true;
            // }
            // else {
            //     tile->active = false;
            // }
        }
    }

    if (InputPressed(Keyboard, Input_UpArrow)) {
        //PlaySound(&Game->audioPlayer, scene->soundClips[2], false);
    }

    {
        PLScene *scene = &Data->scenes[PLSceneID_MoonPelt];
        if (Time - scene->startTime >= 60.0f) {
            Data->startNextScene = true;
        }
    }
}

void PluraLunaInit() {
    Data = (PluraLunaData *)GM.gameData;

    MoonPeltInit(&Data->scenes[PLSceneID_MoonPelt]);

    Data->currScene = PLSceneID_MoonPelt;
}

void PluraLunaUpdate() {
    Data = (PluraLunaData *)GM.gameData;
    
    PLScene *scene = &Data->scenes[Data->currScene];
    void *sceneData = Data->scenes[Data->currScene].data;

    switch (Data->currScene) {
    case PLSceneID_MoonPelt : {
        MoonPeltUpdate(scene, sceneData);
    } break;
    }
}
