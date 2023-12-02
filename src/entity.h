
enum EntityType {
                 EntityType_Entity,
                 EntityType_Player,
                 EntityType_Rock,
                 EntityType_Count,
};

struct EntityInfo {
    EntityType type;
    int32 generation;
    int32 indexInBuffer;
};

struct Entity {
    vec2 position;

    vec2 min;
    vec2 max;

    vec2 center;
    real32 radius;
};


struct EntityManager {
    DynamicArray<EntityInfo> entities;
    DynamicArray_Untyped *buffers;
};
