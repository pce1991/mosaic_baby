
typedef struct {
    int32 seed;
    int32 state;
} LCGState;

#define LCG_MULT 1103515245
#define LCG_INC 12345

// @NOTE: this is global state!!!
LCGState defaultLCGState;

#define MY_RAND_MAX ((1 << 31) - 1)

void SeedRand(int32 seed) {
    defaultLCGState.seed = seed;
    defaultLCGState.state = seed;
}

int32 Randi() {
    defaultLCGState.state = (LCG_MULT * defaultLCGState.state + LCG_INC) & MY_RAND_MAX;

    return (defaultLCGState.state);
}

int32 RandUpper(int32 upperLimit) {
    return Randi() % upperLimit;
}

int32 RandiRange(int32 lowerLimit, int32 upperLimit) {
    int32 range = upperLimit - lowerLimit;
    int32 result = lowerLimit + (Randi() % range);

    return result;
}

real32 Randf() {
    real32 result = (real32)Randi() / (real32)MY_RAND_MAX;
    return result;
}

real32 RandfUpper(real32 upperLimit) {
    real32 max = (real32)MY_RAND_MAX;
    
    real32 percentage = ((real32)Randi() / (real32)MY_RAND_MAX);
    real32 result = percentage * upperLimit;
    return result;
}

real32 RandfRange(real32 lowerLimit, real32 upperLimit) {
    real32 range = upperLimit - lowerLimit;
    real32 result = lowerLimit + RandfUpper(range);
    return result;
}
