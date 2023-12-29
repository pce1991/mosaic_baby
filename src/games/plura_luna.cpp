

enum PLTileState {
     PLTileState_Default,
     PLTileState_Earth,
     PLTileState_Moon,
     PLTileState_Meteor,
     PLTileState_Water,
     PLTileState_Wave,
     PLTileState_MoonTide,
     PLTileState_Sun,
     PLTileState_Space,
     PLTileState_Cloud,
     PLTileState_Lava,
     PLTileState_Critter,
     PLTileState_String,
};

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

    DynamicArray<PLTileState> tileStates;

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


PluraLunaData *PLData = NULL;

void MoonPeltInit(PLScene *scenePtr) {
    scenePtr->data = PushSize(&GM.gameArena, (sizeof(MoonPelt)));

    SetMosaicGridSize(49, 48);

    MoonPelt *moon = (MoonPelt *)scenePtr->data;
    memset(moon, 0, sizeof(MoonPelt));

    moon->moonPos = V2(0, 24);

    moon->moonDir = 1;
    
    LoadSoundClip("data/plura_luna/sleepwalking.wav", &moon->backingClip);

    moon->meteorSounds = MakeDynamicArray<SoundClip>(&GM.gameArena, 20);
    LoadSoundClip("data/plura_luna/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_7.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_8.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_9.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_10.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_11.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_12.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_13.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_3.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_1.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_2.wav", PushBackPtr(&moon->meteorSounds));
    LoadSoundClip("data/plura_luna/meteor_3.wav", PushBackPtr(&moon->meteorSounds));

    moon->craters = MakeDynamicArray<MoonCrater>(&GM.gameArena, 20);

    moon->hoveredTileRadiusDir = 1;
    moon->hoveredTileRadius = 0.1f;

}

void MoonPeltUpdate(PLScene *scenePtr, void *sceneData) {
    PLData = (PluraLunaData *)GM.gameData;
    MoonPelt *scene = (MoonPelt *)sceneData;

    if (scenePtr->firstFrame) {
        PlaySound(&Game->audioPlayer, scene->backingClip, 1.0f);
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

            bool skipSpawn = false;

            // Check if it hits the earth of the moon:
            if (inMoon) {
                crater.offsetPos = Mosaic->hoveredTile->position - moonPosi;    
            }
            else if (inEarth) {
                crater.offsetPos = Mosaic->hoveredTile->position - earthPosi;
                crater.hitEarth = true;
            }
            else {
                skipSpawn = true;
            }
            
            crater.timeHit = Game->time;

            // @TODO: instead od doing this vary the cursor size over time.
            real32 craterRadii[6] = { 0.1f, 0.1f, 0.1f, 1.1f, 1.1f, 2.5f};
            crater.radius = craterRadii[RandiRange(0, 6)];

            crater.radius = hoveredTileRadius;

            SoundClip *clip = NULL;
            clip = &scene->meteorSounds[scene->soundIndex];
            
            PlaySound(&Game->audioPlayer, *clip, 1.0f);

            if (!skipSpawn) {
                PushBack(&scene->craters, crater);
            }
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
        tile->colorB = V4(0);
        //tile->colorB = V4(1, 1, 1, 1.0f) * normNoise;

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

            real32 signedNoise = Sum1f(&Perlin1f, (((Time * 0.2f) + (index * 0.5f))) * 0.2f, 1.0f, 1.0f);
            real32 normNoise = (1 + signedNoise) * 0.5f;

            bool inEarth = false;
            if (distEarth < earthRadius) {
                tile->color = oceanColor;
                tile->color.a = 0.6;

                inEarth = true;
                
                tile->colorB = V4(1, 1, 1, 1.0f) * normNoise;

                vec2 pos = V2(tile->position.x, tile->position.y);

                SetTileSprite(pos, 1);
                SetTileScale(pos, 2.0f);
                SetTileRotation(pos, normNoise * _PI);
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
                tile->colorB = V4(1, 1, 1, 1.0f) * normNoise;

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
        SoundClip clip = scene->meteorSounds[scene->soundIndex];
        PlaySound(&Game->audioPlayer, clip, false);
    }

    {
        PLScene *scene = &PLData->scenes[PLSceneID_MoonPelt];
        if (Time - scene->startTime >= 60.0f) {
            PLData->startNextScene = true;
        }
    }
}


void MoonlightInit(MosaicMem *mosaic, PLScene *scenePtr) {
    scenePtr->data = PushSize(&GM.gameArena, (sizeof(MoonPelt)));
    Moonlight *scene = (Moonlight *)scenePtr->data;

    SetMosaicGridSize(64, 48);

    scenePtr->tileStates = MakeDynamicArray<PLTileState>(&GM.gameArena, Mosaic->tileCapacity);

    For (i, Mosaic->tileCapacity) {
        PushBack(&scenePtr->tileStates, PLTileState_Default);
    }

    scene->depthRange = 16;

    scene->moonPos = V2(11, 11);

    vec2 cloudOrigins[18] = {
                            V2(5, 4),
                            V2(15, 4),
                            V2(25, 4),
                            V2(35, 4),
                            V2(45, 4),
                            V2(55, 4),
                            V2(65, 4),
                            V2(75, 4),
                            V2(95, 4),

                            V2(5, 10),
                            V2(15, 10),
                            V2(25, 10),
                            V2(35, 10),
                            V2(45, 10),
                            V2(55, 10),
                            V2(65, 10),
                            V2(75, 10),
                            V2(95, 10),
    };

    real32 radius = 5;
    for (int i = 0; i < ARRAY_LENGTH(vec2, cloudOrigins); i++) {
        vec2 pos = cloudOrigins[i];

        for (int j = 0; j < 200; j++) {
            Cloud c = {};
            real32 r = RandfRange(0, radius);
            vec2 dir = Normalize(V2(RandfRange(-1, 1),
                                    RandfRange(-1, 1)));
            
            c.pos = pos + (dir * r);

            //c.vel = V2(RandfRange(-1.0f, 1.0f), RandfRange(-1.0f, 1.0f)) * RandfRange(0.2f, 0.4f);
            //c.vel = V2(RandfRange(-1.0f, 1.0f), RandfRange(0, 0)) * RandfRange(0.2f, 0.4f);

            PushBack(&scene->clouds, c);
        }
    }

    {
        Critter c = {};
        c.pos = V2i(5, 40);
        c.color = V4(1);

        PushBack(&scene->critters, c);
    }

    {
        Critter c = {};
        c.pos = V2i(3, 44);
        c.color = V4(0.2f, 0.2f, 0.2f, 1.0f);
        
        PushBack(&scene->critters, c);
    }

    {
        Critter c = {};
        c.pos = V2i(8, 44);
        c.color = V4(0.6f, 0.3f, 0.2f, 1.0f);

        PushBack(&scene->critters, c);
    }

    LoadSoundClip("data/moonlight_song2.wav", &scene->horseSong);
    LoadSoundClip("data/slide_guitar_whole.wav", &scene->windSong);
    // @TODO: generate the light rays, cast them to every pixel, and diminish their intensity as they come down
}

void MoonlightUpdate(MosaicMem *mosaic, PLScene *scenePtr, void *sceneData) {
    Moonlight *scene = (Moonlight *)sceneData;

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        MTile *tile = &Mosaic->tiles[i];
        PLTileState *tileState = &scenePtr->tileStates[i];

        *tileState = PLTileState_Default;
        tile->color = V4(0);

        if (tile->position.y > Mosaic->gridHeight - scene->depthRange) {
            *tileState = PLTileState_Earth;
        }
    }

    if (scenePtr->firstFrame) {
        PlaySound(&Game->audioPlayer, scene->horseSong, false);
        PlaySound(&Game->audioPlayer, scene->windSong, false);
    }

    struct LightRay {
        real32 intensity;
    };

    LightRay rays[64];
    for (int i = 0; i < 64; i++) {
        rays[i].intensity = 1;
    }

    scene->mousePosiPrev = scene->mousePosi;
    MTile *hoveredTile = Mosaic->hoveredTile;
    bool held = InputHeld(Mouse, Input_MouseLeft);
    if (hoveredTile == NULL) { held = false; }
    else { scene->mousePosi = hoveredTile->position; }

    if (held) {
        if (!scene->grab) {
            scene->mouseStartPosi = hoveredTile->position;
            scene->grab = true;
        }
    }
    else {
        scene->grab = false;
    }

    vec2 mouseDiff = V2((scene->mousePosi - scene->mouseStartPosi));
    vec2 pushDir = Normalize(V2((scene->mousePosi - scene->mouseStartPosi)));

    // Problem: the earth is going to band. Each row will always be the same color.
    // The cloud colors are weird because the edges should be bright and centers dark.
    // I want round light.
    // This would actually be a good case of switch between different renders maybe?
    // Could do splitscreen?
    // Treat the clouds as having depth as well? That way we map a coordinate in the sky to the earth
    // That might lead to a checkboard looking pattern. And doesnt seem right because a darker lighter
    // cloud would produce a brighter earth (I guess that isn't wrong)

    // Hearding clouds: if a particle gets too far, it dissipates

    for (int i = 0; i < scene->clouds.count; i++) {
        Cloud *cloud = &scene->clouds[i];
        MTile *tile = GetTile(cloud->pos.x, cloud->pos.y);
        PLTileState *tileState = &scenePtr->tileStates[i];

        if (cloud->dispersed) { continue; }

        if (!tile) {continue;}

        *tileState = PLTileState_Cloud;

        if (held) {
            r32 dist = Distance(tile->position, hoveredTile->position);
            //vec2 dir = Normalize(V2((hoveredTile->position - tile->position)));
            if (dist < 8) {
                cloud->vel = cloud->vel + pushDir * Length(mouseDiff) * 0.1f;
            }

            real32 maxSpeed = 2;
            cloud->vel = Clamp(cloud->vel, V2(-maxSpeed), V2(maxSpeed));

            real32 damp = 4.0f * Game->deltaTime;
            // if (Length(cloud->vel) > damp) {
            //     damp = Length(cloud->vel);
            // }
            cloud->vel = cloud->vel - (cloud->vel * damp);
        }

        cloud->pos = cloud->pos + cloud->vel * Game->deltaTime;
    }
    

    for (int i = 0; i < scene->clouds.count; i++) {
        Cloud *cloud = &scene->clouds[i];
        MTile *tile = GetTile(cloud->pos.x, cloud->pos.y);

        if (cloud->dispersed) { continue; }
        
        if (!tile) {continue;}

        if (tile->position.y > scene->depthRange) {
            cloud->dispersed = true;
            continue;
        }
        
        MTile *neighbors[8] = {};
        GetTileNeighbors(tile, neighbors);

        bool noClouds = true;
        for (int j = 0; j < 8; j++) {
            MTile *n = neighbors[j];
            if (!n) {continue;}

            PLTileState *tileState = &scenePtr->tileStates[GetTileIndex(n->position)];

            if (*tileState == PLTileState_Cloud) {
                noClouds = false;
                break;
            }
        }

        cloud->dispersed = noClouds;
    }
    
    for (int i = 0; i < scene->critters.count; i++) {
        Critter *c = &scene->critters[i];
        MTile *tile = GetTile(c->pos.x, c->pos.y);

        MTile *tile2 = GetTile(c->pos.x - 1, c->pos.y);
        MTile *tile3 = GetTile(c->pos.x - 2, c->pos.y);

        MTile *tile4 = GetTile(c->pos.x, c->pos.y + 1);
        MTile *tile5 = GetTile(c->pos.x - 2, c->pos.y + 1);

        if (tile) {
            tile->color = c->color;

            int32 index = GetTileIndex(c->pos.x, c->pos.y);
            scenePtr->tileStates[index] = PLTileState_Critter;;
        }
        if (tile2) {
            tile2->color = c->color;

            int32 index = GetTileIndex(c->pos.x - 1, c->pos.y);
            scenePtr->tileStates[index] = PLTileState_Critter;;
        }
        if (tile3) {
            tile3->color = c->color;

            int32 index = GetTileIndex(c->pos.x - 2, c->pos.y);
            scenePtr->tileStates[index] = PLTileState_Critter;;
        }

        if (tile4) {
            tile4->color = V4(0.3f, 0.1f, 0.0f, 1.0f);

            int32 index = GetTileIndex(c->pos.x, c->pos.y + 1);
            scenePtr->tileStates[index] = PLTileState_Critter;
        }

        if (tile5) {
            tile5->color = V4(0.3f, 0.1f, 0.0f, 1.0f);

            int32 index = GetTileIndex(c->pos.x - 2, c->pos.y + 1);
            scenePtr->tileStates[index] = PLTileState_Critter;
        }

        MTile *tile6 = GetTile(c->pos.x, c->pos.y - 1);
        if (tile6) {
            tile6->color = c->color;

            int32 index = GetTileIndex(c->pos.x, c->pos.y - 1);
            scenePtr->tileStates[index] = PLTileState_Critter;
        }

        MTile *tile7 = GetTile(c->pos.x + 1, c->pos.y - 1);
        if (tile7) {
            tile7->color = c->color;

            int32 index = GetTileIndex(c->pos.x + 1, c->pos.y - 1);
            scenePtr->tileStates[index] = PLTileState_Critter;
        }
    }

    for (int x = 0; x < Mosaic->gridWidth; x++) {
        LightRay *ray = &rays[x];
        for (int y = 0; y < Mosaic->gridHeight; y++) {
            MTile *tile = GetTile(x, y);
            PLTileState *tileState = &scenePtr->tileStates[GetTileIndex(x, y)];

            int32 index = x + (y * Mosaic->gridHeight);
            real32 signedNoise = Sum1f(&Perlin1f, (((Time * 0.2f) + (index * 0.5f))) * 0.2f, 1.0f, 1.0f);
            real32 normNoise = (1 + signedNoise) * 0.5f;

            if (*tileState == PLTileState_Cloud) {
                ray->intensity -= 0.1f;
            }

            vec2 pos = V2(tile->position.x, tile->position.y);

            SetTileSprite(pos, 1);
            SetTileScale(pos, 2.0f);
            SetTileRotation(pos, normNoise * _PI);

            ray->intensity = Clamp(ray->intensity, 0.3f, 1.0f);

            if (*tileState == PLTileState_Cloud) {
                real32 ambient = 0.4f;
                tile->color = V4(ambient, ambient, ambient, 1.0f) + V4(0.8f, 0.8f, 0.8f, 1.0f) * ray->intensity;
                tile->color.a = ray->intensity;
            }
            else {
                tile->color.a = 1.0f;
            }

            if (*tileState == PLTileState_Earth) {
                MTile *cloudTileAtDepth = GetTile(x, y - scene->depthRange * 2);

                real32 intensity = cloudTileAtDepth->color.a;
                //cloudTileAtDepth->color.a = 1;
                real32 ambient = 0.4f;

                if (intensity < 1) {
                    intensity = 0;
                }
                tile->color = V4(0.0f, 0.1f, 0.0f, 1.0f) + V4(0.1f, 0.2f, 0.2f, 1.0f) * (intensity);
                //tile->color = cloudTileAtDepth->color;
            }

            if (*tileState == PLTileState_Default) {
                real32 t = tile->position.y / (scene->depthRange * 2.0f);
                tile->color = Lerp(V4(0.075f, 0.0f, 0.075f, 1.0f), V4(0.05f, 0.0f, 0.05f, 1.0f), t);
            }
        }
    }


    for (int i = 0; i < scene->critters.count; i++) {
        Critter *c = &scene->critters[i];
        MTile *tile = GetTile(c->pos.x, c->pos.y);

        real32 timeSinceMoved = Game->time - c->timeMoved;
        MTile *front = GetTile(c->pos.x + 1, c->pos.y);

        if (front == NULL) { continue; }
        
        PLTileState *frontState = &scenePtr->tileStates[GetTileIndex(c->pos.x + 1, c->pos.y)];

        MTile *front2 = GetTile(c->pos.x + 2, c->pos.y);
        MTile *cloudTileAtDepth = GetTile(front->position.x, front->position.y - scene->depthRange * 2);
        
        if (!front2) { continue; }

        PLTileState *front2State = &scenePtr->tileStates[GetTileIndex(c->pos.x + 2, c->pos.y)];

        if (*frontState != PLTileState_Critter && *front2State != PLTileState_Critter && cloudTileAtDepth->color.a == 1.0f && timeSinceMoved > 0.4f) {
            c->timeMoved = Game->time;
            c->pos.x += 1;
        }
    }
 
    if (hoveredTile) {
        hoveredTile->color = V4(0.0, 0.4f, 0.8f, 1.0f);
    }

    {
        if (Game->time - scenePtr->startTime >= 36.0f) {
            PLData->startNextScene = true;
        }
    }
}

void PLSetScene(PLSceneID id) {
    PLData = (PluraLunaData *)GM.gameData;
    PLData->currScene = id;

    PLScene *scene = &PLData->scenes[id];
    scene->firstFrame = true;
}

void PluraLunaInit() {
    PLData = (PluraLunaData *)GM.gameData;

    MoonPeltInit(&PLData->scenes[PLSceneID_MoonPelt]);
    MoonlightInit(&PLData->scenes[PLSceneID_Moonlight]);

    PLSetScene(PLSceneID_Moonlight);
}

void PluraLunaUpdate() {
    PLData = (PluraLunaData *)GM.gameData;
    
    PLScene *scene = &PLData->scenes[PLData->currScene];
    void *sceneData = PLData->scenes[PLData->currScene].data;

    switch (PLData->currScene) {
    case PLSceneID_MoonPelt : {
        MoonPeltUpdate(scene, sceneData);
    } break;
    case PLSceneID_Moonlight : {
        MoonlightUpdate(scene, sceneData);
    } break;
    }

    scene->firstFrame = false;
}
