#ifndef H_COMMON
#define H_COMMON

//#define PROFILING
#ifdef PROFILING
    #define STATIC_ITEMS
//    #define PROFILE_FRAMETIME
//    #define PROFILE_SOUNDTIME
#endif

#if defined(_WIN32)
    #define MODE4
    //#define MODE13
    #define USE_DIV_TABLE

    #define _CRT_SECURE_NO_WARNINGS
    #include <windows.h>
#elif defined(__GBA__)
    #define MODE4
    #define USE_DIV_TABLE
    #define ROM_READ

    #include <tonc.h>
#elif defined(__TNS__)
    #define MODE13
    #define USE_DIV_TABLE

    #include <os.h>
#elif defined(__DOS__)
    #define MODE13
    #define USE_DIV_TABLE

    #include <stdlib.h>
    #include <stddef.h>
    #include <conio.h>
    #include <dos.h>
    #include <mem.h>
#elif defined(__3DO__)
    #define MODEHW
    //#define USE_DIV_TABLE // 4k of DRAM
    #define CPU_BIG_ENDIAN

    #define BLOCK_SIZE_DRAM     (32 * 1024)
    #define BLOCK_SIZE_VRAM     (16 * 1024)
    #define BLOCK_SIZE_CD       (2 * 1024)

    #define SND_BUFFER_SIZE     (4 * BLOCK_SIZE_CD)
    #define SND_BUFFERS         4

    #define MAX_RAM_LVL         (BLOCK_SIZE_DRAM * 31) // 35 for LEVEL10C! >_<
    #define MAX_RAM_TEX         (BLOCK_SIZE_VRAM * 44)
    #define MAX_RAM_CEL         (MAX_FACES * sizeof(CCB))
    #define MAX_RAM_SND         (SND_BUFFERS * SND_BUFFER_SIZE)

    extern void* RAM_LVL;
    extern void* RAM_TEX;
    extern void* RAM_CEL;
    extern void* RAM_SND;

    #include <displayutils.h>
    #include <debug.h>
    #include <nodes.h>
    #include <kernelnodes.h>
    #include <list.h>
    #include <folio.h>
    #include <audio.h>
    #include <task.h>
    #include <kernel.h>
    #include <mem.h>
    #include <operamath.h>
    #include <semaphore.h>
    #include <io.h>
    #include <event.h>
    #include <controlpad.h>
    #include <graphics.h>
    #include <celutils.h>
    #include <timerutils.h>
    #include <hardware.h>
    //#include <filefunctions.h>
#else
    #error unsupported platform
#endif

#if !defined(__3DO__)
    #include <string.h>
#endif

#include "stdio.h" // TODO_3DO armcpp bug?

#include <math.h>
#include <limits.h>
#include <new>

#if defined(MODEHW)
    #ifdef __3DO__
        #define FRAME_WIDTH  320
        #define FRAME_HEIGHT 240
    #endif
#elif defined(MODE4)
    #define VRAM_WIDTH   120 // in shorts (240 bytes)
    #define FRAME_WIDTH  240
    #define FRAME_HEIGHT 160
#elif defined(MODE13)
    #define VRAM_WIDTH   160 // in shorts (320 bytes)
    #define FRAME_WIDTH  320

    #if defined(__TNS__)
        #define FRAME_HEIGHT 240 // MODE X?
    #else
        #define FRAME_HEIGHT 200
    #endif
#endif

// Optimization flags =========================================================
#ifdef __GBA__
// hide dead enemies after a while to reduce the number of polygons on the screen
    #define HIDE_CORPSES (30*10) // 10 sec
// replace trap flor geometry by two flat quads in the static state
    #define LOD_TRAP_FLOOR
// disable some plants environment to reduce overdraw of transparent geometry
    #define NO_STATIC_MESH_PLANTS
// disable anim interpolation if item projected box is less than
    #define LOD_ANIM (FRAME_HEIGHT / 4)
// use IWRAM_CODE section that faster for matrix interpolation (GBA only)
    #define IWRAM_MATRIX_LERP
// the maximum of active enemies
//    #define MAX_ENEMIES 3
#endif

#ifdef __3DO__
// hide dead enemies after a while to reduce the number of polygons on the screen
    #define HIDE_CORPSES (30*10) // 10 sec
// replace trap flor geometry by two flat quads in the static state
    #define LOD_TRAP_FLOOR
// disable matrix interpolation
    //#define NO_ANIM_LERP
    #define ANIM_LERP_ANGLE
// the maximum navigation iterations per simulation tick
    #define NAV_STEPS   1
// the maximum of active enemies
    #define MAX_ENEMIES 3
// set the maximum number of simultaneously played channels
    #define SND_CHANNELS 4
#endif

#ifndef NAV_STEPS
    #define NAV_STEPS 5
#endif

#ifndef MAX_ENEMIES
    #define MAX_ENEMIES 8
#endif

#ifndef ANIM_LERP_ANGLE
    #define ANIM_LERP_MATRIX
#endif

// ============================================================================

#if defined(_MSC_VER)
    #define X_INLINE    inline
    #define X_NOINLINE  __declspec(noinline)
    #define ALIGN4      __declspec(align(4))
    #define ALIGN16     __declspec(align(16))
#elif defined(__WATCOMC__) || defined(__3DO__)
    #define X_INLINE    inline
    #define X_NOINLINE  __declspec(noinline)
    #define ALIGN4
    #define ALIGN16
#else
    #define X_INLINE    __attribute__((always_inline)) inline
    #define X_NOINLINE  __attribute__((noinline))
    #define ALIGN4      __attribute__((aligned(4)))
    #define ALIGN16     __attribute__((aligned(16)))
#endif

#if defined(__3DO__)
typedef size_t             intptr_t;
typedef uint32             divTableInt;
#else
typedef signed char        int8;
typedef signed short       int16;
typedef signed int         int32;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef uint16             divTableInt;
#endif

typedef int16              Index;

#if defined(__3DO__)
X_INLINE int32 abs(int32 x) {
    return (x >= 0) ? x : -x;
}
#endif

#ifdef __GBA__
    #define ARM_CODE __attribute__((target("arm")))
#else
    #define ARM_CODE
    #define THUMB_CODE
    #define IWRAM_DATA
    #define EWRAM_DATA
    #define EWRAM_BSS
    #define IWRAM_CODE
    #define EWRAM_CODE

    #define dmaCopy(src,dst,size) memcpy(dst,src,size)
#endif

#if defined(_WIN32)
    #define ASSERT(x) { if (!(x)) { DebugBreak(); } }
#else
    #define ASSERT(x)
#endif

#if defined(__GBA__)
    #define IME_DISABLE() u16 origIME = REG_IME; REG_IME = 0
    #define IME_ENABLE()  REG_IME = origIME;
#else
    #define IME_DISABLE()
    #define IME_ENABLE()
#endif

#if defined(_WIN32)
    extern uint16 fb[VRAM_WIDTH * FRAME_HEIGHT];
#elif defined(__GBA__)
    extern uint32 fb;
#elif defined(__TNS__)
    extern uint16 fb[VRAM_WIDTH * FRAME_HEIGHT];
#elif defined(__DOS__)
    extern uint16 fb[VRAM_WIDTH * FRAME_HEIGHT];
#endif

// system
extern int32 osGetSystemTimeMS();
extern void osJoyVibrate(int32 index, int32 L, int32 R);
extern void osSetPalette(const uint16* palette);
extern void* osLoadLevel(const char* name);

#ifdef PROFILING
    #define PROFILE_FRAME\
        CNT_UPDATE,\
        CNT_RENDER

    #define PROFILE_STAGES\
        CNT_TRANSFORM,\
        CNT_ADD,\
        CNT_FLUSH,\
        CNT_VERT,\
        CNT_POLY

    #define PROFILE_SOUND\
        CNT_SOUND

    #if defined(PROFILE_FRAMETIME)
        enum ProfileCounterId {
            PROFILE_FRAME,
            CNT_MAX,
            PROFILE_STAGES,
            PROFILE_SOUND
        };
    #elif defined(PROFILE_SOUNDTIME)
        enum ProfileCounterId {
            PROFILE_SOUND,
            CNT_MAX,
            PROFILE_FRAME,
            PROFILE_STAGES
        };
    #else
        enum ProfileCounterId {
            PROFILE_STAGES,
            CNT_MAX,
            PROFILE_FRAME,
            PROFILE_SOUND
        };
    #endif

    extern uint32 gCounters[CNT_MAX];
    
    #if defined(__3DO__) // should be first, armcpp bug (#elif)
        extern int32 g_timer;

        #define PROFILE_START() {\
            g_timer = osGetSystemTimeMS();\
        }

        #define PROFILE_STOP(value) {\
            value += (osGetSystemTimeMS() - g_timer);\
        }
    #elif defined(_WIN32)
        extern LARGE_INTEGER g_timer;
        extern LARGE_INTEGER g_current;

        #define PROFILE_START() {\
            QueryPerformanceCounter(&g_timer);\
        }

        #define PROFILE_STOP(value) {\
            QueryPerformanceCounter(&g_current);\
            value += (g_current.QuadPart - g_timer.QuadPart);\
        }
    #elif defined(__GBA__)
        #ifdef PROFILE_SOUNDTIME
            #define TIMER_FREQ_DIV 1
        #else
            #define TIMER_FREQ_DIV 3
        #endif

        #define PROFILE_START() {\
            REG_TM2CNT_L = 0;\
            REG_TM2CNT_H = (1 << 7) | TIMER_FREQ_DIV;\
        }

        #define PROFILE_STOP(value) {\
            value += REG_TM2CNT_L;\
            REG_TM2CNT_H = 0;\
        }
    #else
        #define PROFILE_START() aaa
        #define PROFILE_STOP(value) bbb
    #endif

    struct ProfileCounter
    {
        ProfileCounterId cnt;

        ProfileCounter(ProfileCounterId cnt) : cnt(cnt) {
            if (cnt < CNT_MAX) {
                PROFILE_START()
            }
        }

        ~ProfileCounter() {
            if (cnt < CNT_MAX) {
                PROFILE_STOP(gCounters[cnt]);
            }
        }
    };

    #define PROFILE(cnt) ProfileCounter profileCounter(cnt)
    #define PROFILE_CLEAR() memset(gCounters, 0, sizeof(gCounters));
#else
    #define PROFILE(cnt)
    #define PROFILE_CLEAR()
#endif

#ifdef __TNS__
    void osSetPalette(uint16* palette);
#endif

#define STATIC_MESH_FLAG_NO_COLLISION   1
#define STATIC_MESH_FLAG_VISIBLE        2

extern int32 fps;

#define FIXED_SHIFT     14

#define SND_MAX_DIST    (8 * 1024)

#ifndef SND_CHANNELS
    #define SND_CHANNELS 6
#endif

#define SND_FIXED_SHIFT 8
#define SND_VOL_SHIFT   6
#define SND_PITCH_SHIFT 7

#if defined(_WIN32)
    #define SND_SAMPLES      1024
    #define SND_OUTPUT_FREQ  22050
    #define SND_SAMPLE_FREQ  22050
    #define SND_ENCODE(x)    ((x) + 128)
    #define SND_DECODE(x)    ((x) - 128)
    #define SND_MIN          -128
    #define SND_MAX          127
#elif defined(__GBA__)
    #define SND_SAMPLES      176
    #define SND_OUTPUT_FREQ  10512
    #define SND_SAMPLE_FREQ  22050
    #define SND_ENCODE(x)    (x)
    #define SND_DECODE(x)    ((x) - 128)
    #define SND_MIN          -128
    #define SND_MAX          127
#elif defined(__DOS__)
    #define SND_SAMPLES      1024
    #define SND_OUTPUT_FREQ  11025
    #define SND_SAMPLE_FREQ  11025
    #define SND_ENCODE(x)    ((x) + 128)
    #define SND_DECODE(x)    ((x) - 128)
    #define SND_MIN          -128
    #define SND_MAX          127
#elif defined(__3DO__)
    #define SND_SAMPLES      1024
    #define SND_OUTPUT_FREQ  11025
    #define SND_SAMPLE_FREQ  11025
    #define SND_ENCODE(x)    ((x) + 128)
    #define SND_DECODE(x)    ((x) - 128)
    #define SND_MIN          -128
    #define SND_MAX          127
#endif

#ifdef __3DO__
    #define MAX_VERTICES (1024 + 32) // for mesh (max = LEVEL10A room:58)
#else
    #define MAX_VERTICES (5*1024) // for frame
#endif

#define MAX_UPDATE_FRAMES 10

#define MAX_PLAYERS         1 // TODO 2 players for non-potato platforms
#define MAX_SPHERES         32
#define MAX_MATRICES        8
#define MAX_ROOMS           139 // LEVEL7A
#define MAX_ITEMS           256
#define MAX_MODELS          ITEM_MAX
#define MAX_MESHES          512
#define MAX_STATIC_MESHES   50
#define MAX_CAMERAS         16
#define MAX_BOXES           1024
#define MAX_TEXTURES        1536
#define MAX_FACES           1920
#define MAX_ROOM_LIST       16
#define MAX_PORTALS         16
#define MAX_CAUSTICS        32
#define MAX_RAND_TABLE      32
#define MAX_DYN_SECTORS     (1024*3)
#define MAX_SAMPLES         180

#define FOV_SHIFT       3
#define FOG_SHIFT       1
#define FOG_MAX         (10 * 1024)
#define FOG_MIN         (FOG_MAX - (8192 >> FOG_SHIFT))
#define VIEW_MIN_F      (32 << FIXED_SHIFT)
#define VIEW_MAX_F      (FOG_MAX << FIXED_SHIFT)

#define FRUSTUM_FAR_X   5 << 10
#define FRUSTUM_FAR_Y   3 << 10
#define FRUSTUM_FAR_Z   9 << 10

#define FACE_TRIANGLE   0x8000
#define FACE_COLORED    0x4000
#define FACE_CLIPPED    0x2000
#define FACE_FLAT       0x1000
#define FACE_SPRITE     0x0800
#define FACE_SHADOW     (FACE_COLORED | FACE_FLAT | FACE_SPRITE)
#define FACE_TEXTURE    0x07FF

#define FACE_CCW        (1 << 31) // 3DO only
#define FACE_MIP_SHIFT  11        // 3DO only

#define NOT_ENEMY       -0x4000     // default hp for non enemies
#define NO_ROOM         0xFF
#define NO_MODEL        0xFF
#define NO_BOX          0xFFFF
#define NO_FLOOR        -127
#define WALL            (NO_FLOOR * 256)

#define ANGLE_0         0
#define ANGLE_1         (0x10000 / 360)
#define ANGLE_45        (0x10000 / 8)      // != 45 * ANGLE_1 !!!
#define ANGLE_90        (0x10000 / 4)      // != 90 * ANGLE_1 !!!
#define ANGLE_180      -(0x10000 / 2)      // INT16_MIN
#define ANGLE(x)        ((x) * ANGLE_1)
#define ANGLE_SHIFT_45  13
#define ANGLE_SHIFT_90  14
#define ANGLE_SHIFT_180 15

#define X_CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define X_MIN(a,b)       ((a) < (b) ? (a) : (b))
#define X_MAX(a,b)       ((a) > (b) ? (a) : (b))
#define X_SQR(x)         ((x) * (x))
#define X_COUNT(x)       int32(sizeof(x) / sizeof(x[0]))

#define X_ROTX(x,y,s,c)  (((x) * (c) - (y) * (s)) >> FIXED_SHIFT)
#define X_ROTY(x,y,s,c)  (((y) * (c) + (x) * (s)) >> FIXED_SHIFT)
#define X_ROTXY(x,y,s,c) {\
    int32 _x = X_ROTX(x,y,s,c);\
    int32 _y = X_ROTY(x,y,s,c);\
    x = _x;\
    y = _y;\
}

#define DP43(ax,ay,az,aw,bx,by,bz)  (ax * bx + ay * by + az * bz + aw)
#define DP33(ax,ay,az,bx,by,bz)     (ax * bx + ay * by + az * bz)

#ifdef USE_DIV_TABLE
    #define DIV_TABLE_SIZE   1024
    #define FixedInvS(x)     ((x < 0) ? -divTable[abs(x)] : divTable[x])
    #define FixedInvU(x)     divTable[x]
    extern divTableInt divTable[DIV_TABLE_SIZE];

    #define GET_FRAME_T(x,n) (FixedInvU(n) * x)
#else
    #define GET_FRAME_T(x,n) ((x << 16) / n)
#endif

#define OT_SHIFT        4
#define OT_SIZE         ((VIEW_MAX_F >> (FIXED_SHIFT + OT_SHIFT)) + 1)

// system keys (keys)
enum InputKey {
    IK_NONE     = 0,
    IK_UP       = (1 << 0),
    IK_RIGHT    = (1 << 1),
    IK_DOWN     = (1 << 2),
    IK_LEFT     = (1 << 3),
    IK_A        = (1 << 4),
    IK_B        = (1 << 5),
    IK_C        = (1 << 6),
    IK_L        = (1 << 7),
    IK_R        = (1 << 8),
    IK_START    = (1 << 9),
    IK_SELECT   = (1 << 10)
};

// action keys (ItemObj::input)
enum {
    IN_LEFT   = (1 << 1),
    IN_RIGHT  = (1 << 2),
    IN_UP     = (1 << 3),
    IN_DOWN   = (1 << 4),
    IN_JUMP   = (1 << 5),
    IN_WALK   = (1 << 6),
    IN_ACTION = (1 << 7),
    IN_WEAPON = (1 << 8),
    IN_LOOK   = (1 << 9)
};

struct vec3s {
    int16 x, y, z;

    X_INLINE static vec3s create(int16 x, int16 y, int16 z) {
        vec3s r;
        r.x = x;
        r.y = y;
        r.z = z;
        return r;
    }

    X_INLINE vec3s  operator + (const vec3s &v) const { return create(x + v.x, y + v.y, z + v.z); }
    X_INLINE vec3s  operator - (const vec3s &v) const { return create(x - v.x, y - v.y, z - v.z); }
    X_INLINE bool   operator == (const vec3s &v) { return x == v.x && y == v.y && z == v.z; }
    X_INLINE bool   operator != (const vec3s &v) { return x != v.x || y != v.y || z != v.z; }
    X_INLINE vec3s& operator += (const vec3s &v) { x += v.x; y += v.y; z += v.z; return *this; }
    X_INLINE vec3s& operator -= (const vec3s &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
};

#define _vec3s(x,y,z) vec3s::create(x, y, z)


struct vec3i {
    int32 x, y, z;

    X_INLINE static vec3i create(int32 x, int32 y, int32 z) {
        vec3i r;
        r.x = x;
        r.y = y;
        r.z = z;
        return r;
    }

    X_INLINE vec3i  operator + (const vec3i &v) const { return create(x + v.x, y + v.y, z + v.z); }
    X_INLINE vec3i  operator - (const vec3i &v) const { return create(x - v.x, y - v.y, z - v.z); }
    X_INLINE vec3i  operator * (int32 s) const { return create(x * s, y * s, z * s); }
    X_INLINE vec3i  operator / (int32 s) const { return create(x / s, y / s, z / s); }
    X_INLINE bool   operator == (const vec3i &v) const { return x == v.x && y == v.y && z == v.z; }
    X_INLINE bool   operator != (const vec3i &v) const { return x != v.x || y != v.y || z != v.z; }
    X_INLINE vec3i& operator += (const vec3i &v) { x += v.x; y += v.y; z += v.z; return *this; }
    X_INLINE vec3i& operator -= (const vec3i &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    X_INLINE vec3i& operator *= (int32 s) { x *= s; y *= s; z *= s; return *this; }
    X_INLINE vec3i& operator /= (int32 s) { x /= s; y /= s; z /= s; return *this; }
};

#define _vec3i(x,y,z) vec3i::create(x, y, z)

struct vec4i {
    int32 x, y, z, w;

    X_INLINE int32& operator [] (int32 index) const {
        ASSERT(index >= 0 && index <= 3);
        return ((int32*)this)[index];
    }
};

#ifdef __3DO__
    #define F16_SHIFT (16 - FIXED_SHIFT)    // for fix14<->fix16 conversion
    #define DOT_SHIFT (FIXED_SHIFT + FIXED_SHIFT - 16 - F16_SHIFT)
#endif

struct Matrix
{
#ifdef __3DO__
    int32 e00, e10, e20;
    int32 e01, e11, e21;
    int32 e02, e12, e22;
    int32 e03, e13, e23;
#else
    int32 e00, e01, e02, e03;
    int32 e10, e11, e12, e13;
    int32 e20, e21, e22, e23;
#endif
};

struct RoomQuad
{
#ifdef __3DO__
    Index  indices[4];
    uint32 flags;
#else
    Index  indices[4];
    uint16 flags;
#endif
};

struct MeshQuad
{
#ifdef __3DO__
    uint32 indices;
    uint32 flags;
#else
    Index  indices[4];
    uint16 flags;
#endif
};

struct RoomTriangle
{
#ifdef __3DO__
    Index  indices[3];
    uint16 _unused;
    uint32 flags;
#else
    Index  indices[3];
    uint16 flags;
#endif
};

struct MeshTriangle
{
#ifdef __3DO__
    uint32 indices;
    uint32 flags;
#else
    Index  indices[3];
    uint16 flags;
#endif
};

struct RectMinMax
{
    int32 x0;
    int32 y0;
    int32 x1;
    int32 y1;

    RectMinMax() {}
    RectMinMax(int32 x0, int32 y0, int32 x1, int32 y1) : x0(x0), y0(y0), x1(x1), y1(y1) {}
};

union TexCoord
{
    struct { uint16 v, u; } uv;
    uint32 t;
};

#ifdef __3DO__
typedef CCB Face;
#else
struct Face
{
    Face* next;
    uint16 flags;
    int16 indices[4];
};
#endif

struct AABBs
{
    int16 minX;
    int16 maxX;
    int16 minY;
    int16 maxY;
    int16 minZ;
    int16 maxZ;

    X_INLINE AABBs() {}
    X_INLINE AABBs(int16 minX, int16 maxX, int16 minY, int16 maxY, int16 minZ, int16 maxZ) :
        minX(minX), maxX(maxX), minY(minY), maxY(maxY), minZ(minZ), maxZ(maxZ) {}

    X_INLINE vec3i getCenter() const {
        return _vec3i((maxX + minX) >> 1, (maxY + minY) >> 1, (maxZ + minZ) >> 1);
    }
};

struct AABBi
{
    int32 minX;
    int32 maxX;
    int32 minY;
    int32 maxY;
    int32 minZ;
    int32 maxZ;
};

struct Sphere
{
    vec3i center;
    int32 radius;
};

struct Room;

struct RoomVertex
{
    int8 x, y, z;
    uint8 g;
};

struct RoomSprite
{
    vec3s pos;
    uint8 g;
    uint8 index;
};

struct MeshVertex
{
#ifdef __3DO__
    int16 y, x, _unused, z;
#else
    int16 x, y, z;
#endif
};

struct Portal
{
#ifdef __3DO__
    uint32 roomIndex;
    uint32 normalMask;
    vec3i v[4];
#else
    uint16 roomIndex;
    vec3s n;
    vec3s v[4];
#endif
};

struct Sector
{
    uint16 floorIndex;
    uint16 boxIndex;
    uint8  roomBelow;
    int8   floor;
    uint8  roomAbove;
    int8   ceiling;

    const Sector* getSectorBelow(int32 posX, int32 posZ) const;
    const Sector* getSectorAbove(int32 posX, int32 posZ) const;
    int32 getFloor(int32 x, int32 y, int32 z) const;
    int32 getCeiling(int32 x, int32 y, int32 z) const;
    Room* getNextRoom() const;
    void getTriggerFloorCeiling(int32 x, int32 y, int32 z, int32* floor, int32* ceiling) const;
};

struct Light
{
    vec3s pos;
    uint8 radius;
    uint8 intensity;
};

#define STATIC_MESH_ID(flags)  ((flags) & 0x3F)
#define STATIC_MESH_ROT(flags) ((((flags) >> 6) - 2) * ANGLE_90)

struct RoomMesh
{
    vec3s pos;
    uint8 intensity;
    uint8 flags;
};

struct ItemObj;

struct RoomData
{
    const RoomQuad* quads;
    const RoomTriangle* triangles;
    const RoomVertex* vertices;
    const RoomSprite* sprites;
    const Portal* portals;
    const Sector* sectors;
    const Light* lights;
    const RoomMesh* meshes;
};

#define ROOM_FLAG_WATER(x) ((x) & 1)

struct RoomInfo
{
    int16 x;
    int16 z;

    int16 yBottom;
    int16 yTop;

    uint16 quadsCount;
    uint16 trianglesCount;
    
    uint16 verticesCount;
    uint16 spritesCount;

    uint8 portalsCount;
    uint8 lightsCount;
    uint8 meshesCount;
    uint8 ambient;

    uint8 xSectors;
    uint8 zSectors;
    uint8 alternateRoom;
    uint8 flags;

    RoomData data;
};

struct CollisionInfo;

struct Room {
    ItemObj* firstItem;
    const RoomInfo* info;
    const Sector* sectors; // == info->sectors (const) by default (see roomModify) 

    RoomData data;

    RectMinMax clip;
    uint8 _reserved;
    bool visible;

    void modify();
    void reset();

    void add(ItemObj* item);
    void remove(ItemObj* item);

    const Sector* getSector(int32 x, int32 z) const;
    const Sector* getWaterSector(int32 x, int32 z) const;
    Room* getRoom(int32 x, int32 y, int32 z);
    bool collideStatic(CollisionInfo &cinfo, const vec3i &p, int32 height);
#ifdef __3DO__
    bool checkPortal(const Portal* portal, vec3i* points);
#else
    bool checkPortal(const Portal* portal);
#endif
    Room** addVisibleRoom(Room** list);
    Room** addNearRoom(Room** list, int32 x, int32 y, int32 z);
    Room** getNearRooms(const vec3i &pos, int32 radius, int32 height);
    Room** getAdjRooms();
    Room** getVisibleRooms();
};

enum NodeFlag {
    NODE_FLAG_POP  = (1 << 0),
    NODE_FLAG_PUSH = (1 << 1),
    NODE_FLAG_ROTX = (1 << 2),
    NODE_FLAG_ROTY = (1 << 3),
    NODE_FLAG_ROTZ = (1 << 4)
};

struct ModelNode {
    uint16 flags;
    vec3s pos;
};

struct Model {
    uint8 type;
    int8 count;
    uint16 start;
    uint16 nodeIndex;
    uint16 animIndex;
};

#define FILE_MODEL_SIZE (sizeof(Model) - 2) // -padding

struct Mesh {
    vec3s center;
    int16 radius;
    uint16 flags;
    int16 vCount;
    int16 rCount;
    int16 tCount;
    int16 crCount;
    int16 ctCount;
    // data...
};

struct StaticMesh {
    uint16 id;
    uint16 meshIndex;
    uint16 flags;
    AABBs vbox;
    AABBs cbox;
};

#define SI_MODE(x)      (x & 3)
#define SI_COUNT(x)     ((x >> 2) & 15)
#define SI_CAMERA(x)    ((x >> 12) & 1)
#define SI_PITCH(x)     ((x >> 13) & 1)
#define SI_GAIN(x)      ((x >> 14) & 1)

struct SoundInfo
{
    uint16 index;
    uint16 volume;
    uint16 chance;
    uint16 flags;
};

struct Anim {
    uint32 frameOffset;
    uint8 frameRate;
    uint8 frameSize;
    uint16 state;
    int32 speed;
    int32 accel;
    uint16 frameBegin;
    uint16 frameEnd;
    uint16 nextAnimIndex;
    uint16 nextFrameIndex;
    uint16 statesCount;
    uint16 statesStart;
    uint16 commandsCount;
    uint16 commandsStart;
};

struct AnimState {
    uint8 state;
    uint8 rangesCount;
    uint16 rangesStart;
};

struct AnimRange {
    uint16 frameBegin;
    uint16 frameEnd;
    uint16 nextAnimIndex;
    uint16 nextFrameIndex;
};

struct AnimFrame {
    AABBs box;
    vec3s pos;
    uint16 angles[1];
};

struct Texture
{
#ifdef __3DO__
    uint8* data;
    uint8* plut;
    uint32 shift;
#else
    uint16 attribute;
    uint16 tile;
    uint32 uv0;
    uint32 uv1;
    uint32 uv2;
    uint32 uv3;
#endif
};

struct Sprite
{
#ifdef __3DO__
    uint32 texture;
    int32 l, t, r, b;
#else
    uint16 tile;
    uint8 u, v;
    uint8 w, h;
    int16 l, t, r, b;
#endif
};

struct SpriteSeq {
    uint16 type;
    uint16 unused;
    int16 count;
    uint16 start;
};

struct FixedCamera {
    vec3i pos;
    int16 roomIndex;
    struct {
        uint16 timer:8, once:1, speed:5, :2;
    } flags;
};

struct ItemObjFlags {
    uint16 save:1, gravity:1, active:1, status:2, collision:1, injured:1, animated:1, once:1, mask:5, reverse:1, shadow:1; // TODO
};

struct ItemObjInfo {
    uint8 type;
    uint8 roomIndex;
    vec3s pos;
    uint16 intensity;

    union {
        ItemObjFlags flags;
        uint16 value;
    } params;
};

#define ITEM_FLAGS_MASK_ALL         0x1F

#define ITEM_FLAGS_STATUS_NONE      0
#define ITEM_FLAGS_STATUS_ACTIVE    1
#define ITEM_FLAGS_STATUS_INACTIVE  2
#define ITEM_FLAGS_STATUS_INVISIBLE 3

#define FILE_ITEM_SIZE (sizeof(ItemObjInfo) - 2)

struct CollisionInfo;
struct Lara;

#define ITEM_TYPES(E) \
    E( LARA                  ) \
    E( LARA_PISTOLS          ) \
    E( LARA_SHOTGUN          ) \
    E( LARA_MAGNUMS          ) \
    E( LARA_UZIS             ) \
    E( LARA_SPEC             ) \
    E( DOPPELGANGER          ) \
    E( WOLF                  ) \
    E( BEAR                  ) \
    E( BAT                   ) \
    E( CROCODILE_LAND        ) \
    E( CROCODILE_WATER       ) \
    E( LION_MALE             ) \
    E( LION_FEMALE           ) \
    E( PUMA                  ) \
    E( GORILLA               ) \
    E( RAT_LAND              ) \
    E( RAT_WATER             ) \
    E( REX                   ) \
    E( RAPTOR                ) \
    E( MUTANT_1              ) \
    E( MUTANT_2              ) \
    E( MUTANT_3              ) \
    E( CENTAUR               ) \
    E( MUMMY                 ) \
    E( UNUSED_1              ) \
    E( UNUSED_2              ) \
    E( LARSON                ) \
    E( PIERRE                ) \
    E( SKATEBOARD            ) \
    E( SKATER                ) \
    E( COWBOY                ) \
    E( MR_T                  ) \
    E( NATLA                 ) \
    E( ADAM                  ) \
    E( TRAP_FLOOR            ) \
    E( TRAP_SWING_BLADE      ) \
    E( TRAP_SPIKES           ) \
    E( TRAP_BOULDER          ) \
    E( DART                  ) \
    E( TRAP_DART_EMITTER     ) \
    E( DRAWBRIDGE            ) \
    E( TRAP_SLAM             ) \
    E( TRAP_SWORD            ) \
    E( HAMMER_HANDLE         ) \
    E( HAMMER_BLOCK          ) \
    E( LIGHTNING             ) \
    E( MOVING_OBJECT         ) \
    E( BLOCK_1               ) \
    E( BLOCK_2               ) \
    E( BLOCK_3               ) \
    E( BLOCK_4               ) \
    E( MOVING_BLOCK          ) \
    E( TRAP_CEILING          ) \
    E( TRAP_FLOOR_LOD        ) \
    E( SWITCH                ) \
    E( SWITCH_WATER          ) \
    E( DOOR_1                ) \
    E( DOOR_2                ) \
    E( DOOR_3                ) \
    E( DOOR_4                ) \
    E( DOOR_5                ) \
    E( DOOR_6                ) \
    E( DOOR_7                ) \
    E( DOOR_8                ) \
    E( TRAP_DOOR_1           ) \
    E( TRAP_DOOR_2           ) \
    E( TRAP_DOOR_LOD         ) \
    E( BRIDGE_FLAT           ) \
    E( BRIDGE_TILT_1         ) \
    E( BRIDGE_TILT_2         ) \
    E( INV_PASSPORT          ) \
    E( INV_COMPASS           ) \
    E( INV_HOME              ) \
    E( GEARS_1               ) \
    E( GEARS_2               ) \
    E( GEARS_3               ) \
    E( CUT_1                 ) \
    E( CUT_2                 ) \
    E( CUT_3                 ) \
    E( CUT_4                 ) \
    E( INV_PASSPORT_CLOSED   ) \
    E( INV_MAP               ) \
    E( CRYSTAL               ) \
    E( PISTOLS               ) \
    E( SHOTGUN               ) \
    E( MAGNUMS               ) \
    E( UZIS                  ) \
    E( AMMO_PISTOLS          ) \
    E( AMMO_SHOTGUN          ) \
    E( AMMO_MAGNUMS          ) \
    E( AMMO_UZIS             ) \
    E( EXPLOSIVE             ) \
    E( MEDIKIT_SMALL         ) \
    E( MEDIKIT_BIG           ) \
    E( INV_DETAIL            ) \
    E( INV_SOUND             ) \
    E( INV_CONTROLS          ) \
    E( INV_GAMMA             ) \
    E( INV_PISTOLS           ) \
    E( INV_SHOTGUN           ) \
    E( INV_MAGNUMS           ) \
    E( INV_UZIS              ) \
    E( INV_AMMO_PISTOLS      ) \
    E( INV_AMMO_SHOTGUN      ) \
    E( INV_AMMO_MAGNUMS      ) \
    E( INV_AMMO_UZIS         ) \
    E( INV_EXPLOSIVE         ) \
    E( INV_MEDIKIT_SMALL     ) \
    E( INV_MEDIKIT_BIG       ) \
    E( PUZZLE_1              ) \
    E( PUZZLE_2              ) \
    E( PUZZLE_3              ) \
    E( PUZZLE_4              ) \
    E( INV_PUZZLE_1          ) \
    E( INV_PUZZLE_2          ) \
    E( INV_PUZZLE_3          ) \
    E( INV_PUZZLE_4          ) \
    E( PUZZLEHOLE_1          ) \
    E( PUZZLEHOLE_2          ) \
    E( PUZZLEHOLE_3          ) \
    E( PUZZLEHOLE_4          ) \
    E( PUZZLEHOLE_DONE_1     ) \
    E( PUZZLEHOLE_DONE_2     ) \
    E( PUZZLEHOLE_DONE_3     ) \
    E( PUZZLEHOLE_DONE_4     ) \
    E( LEADBAR               ) \
    E( INV_LEADBAR           ) \
    E( MIDAS_HAND            ) \
    E( KEY_ITEM_1            ) \
    E( KEY_ITEM_2            ) \
    E( KEY_ITEM_3            ) \
    E( KEY_ITEM_4            ) \
    E( INV_KEY_ITEM_1        ) \
    E( INV_KEY_ITEM_2        ) \
    E( INV_KEY_ITEM_3        ) \
    E( INV_KEY_ITEM_4        ) \
    E( KEYHOLE_1             ) \
    E( KEYHOLE_2             ) \
    E( KEYHOLE_3             ) \
    E( KEYHOLE_4             ) \
    E( UNUSED_5              ) \
    E( UNUSED_6              ) \
    E( SCION_PICKUP_QUALOPEC ) \
    E( SCION_PICKUP_DROP     ) \
    E( SCION_TARGET          ) \
    E( SCION_PICKUP_HOLDER   ) \
    E( SCION_HOLDER          ) \
    E( UNUSED_7              ) \
    E( UNUSED_8              ) \
    E( INV_SCION             ) \
    E( EXPLOSION             ) \
    E( UNUSED_9              ) \
    E( SPLASH                ) \
    E( UNUSED_10             ) \
    E( BUBBLE                ) \
    E( UNUSED_11             ) \
    E( UNUSED_12             ) \
    E( BLOOD                 ) \
    E( UNUSED_13             ) \
    E( SMOKE                 ) \
    E( CENTAUR_STATUE        ) \
    E( CABIN                 ) \
    E( MUTANT_EGG_SMALL      ) \
    E( RICOCHET              ) \
    E( SPARKLES              ) \
    E( MUZZLE_FLASH          ) \
    E( UNUSED_14             ) \
    E( UNUSED_15             ) \
    E( VIEW_TARGET           ) \
    E( WATERFALL             ) \
    E( NATLA_BULLET          ) \
    E( MUTANT_BULLET         ) \
    E( CENTAUR_BULLET        ) \
    E( UNUSED_16             ) \
    E( UNUSED_17             ) \
    E( LAVA_PARTICLE         ) \
    E( LAVA_EMITTER          ) \
    E( FLAME                 ) \
    E( FLAME_EMITTER         ) \
    E( TRAP_LAVA             ) \
    E( MUTANT_EGG_BIG        ) \
    E( BOAT                  ) \
    E( EARTHQUAKE            ) \
    E( UNUSED_18             ) \
    E( UNUSED_19             ) \
    E( UNUSED_20             ) \
    E( UNUSED_21             ) \
    E( UNUSED_22             ) \
    E( LARA_BRAID            ) \
    E( GLYPHS                )

#define DECL_ENUM(v) ITEM_##v,

enum ItemType {
    ITEM_TYPES(DECL_ENUM)
    TR1_ITEM_MAX,
    ITEM_MAX = TR1_ITEM_MAX
};

#undef DECL_ENUM

struct Location {
    Room* room;
    vec3i pos;
};

enum CameraMode {
    CAMERA_MODE_FREE,
    CAMERA_MODE_FOLLOW,
    CAMERA_MODE_COMBAT,
    CAMERA_MODE_LOOK,
    CAMERA_MODE_FIXED,
    CAMERA_MODE_OBJECT,
    CAMERA_MODE_CUTSCENE
};

struct Camera {
    Location view;
    Location target;

    int32 targetDist;
    vec3s targetAngle;

    vec3s angle;

    AABBi  frustumBase;

    ItemObj* laraItem;
    ItemObj* lastItem;
    ItemObj* lookAtItem;

    int32 speed;
    int32 timer;
    int32 index;
    int32 lastIndex;

    CameraMode mode;

    bool modeSwitch;
    bool lastFixed;
    bool center;

    void init(ItemObj* lara);
    Location getLocationForAngle(int32 angle, int32 distH, int32 distV);
    void clip(Location &loc);
    Location getBestLocation(bool clip);
    void move(Location &to, int32 speed);
    void updateFree();
    void updateFollow(ItemObj* item);
    void updateCombat(ItemObj* item);
    void updateLook(ItemObj* item);
    void updateFixed();
    void lookAt(int32 offset);
    void update();
    void prepareFrustum();
    void updateFrustum(int32 offsetX, int32 offsetY, int32 offsetZ);
    void toCombat();
};

enum ZoneType
{
    ZONE_GROUND_1,
    ZONE_GROUND_2,
    ZONE_FLY,
    ZONE_MAX
};

struct Nav
{
    struct Cell
    {
        uint16 boxIndex;
        uint16 weight;
        uint16 end;
        uint16 next;
    };

    Cell cells[MAX_BOXES];
    uint32 cellsCount;

    uint32 zoneType;
    uint32 weight;

    uint32 endBox;
    uint32 nextBox;
    uint32 headBox;
    uint32 tailBox;
    int32 stepHeight;
    int32 dropHeight;
    int32 vSpeed;
    uint32 mask;

    vec3i pos;

    void init(uint32 boxIndex);
    void search(uint16 zone, const uint16* zones);
    vec3i getWaypoint(uint32 boxIndex, const vec3i &from);
};

enum WeaponState
{
    WEAPON_STATE_FREE,
    WEAPON_STATE_BUSY,
    WEAPON_STATE_DRAW,
    WEAPON_STATE_HOLSTER,
    WEAPON_STATE_READY
};

enum Weapon
{
    WEAPON_PISTOLS,
    WEAPON_MAGNUMS,
    WEAPON_UZIS,
    WEAPON_SHOTGUN,
    // WEAPON_DESERT_EAGLE,
    // WEAPON_REVOLVER,
    // WEAPON_M16
    // WEAPON_MP5
    // WEAPON_HK
    // WEAPON_ROCKET
    // WEAPON_GRENADE
    // WEAPON_HARPOON
    // WEAPON_CROSSBOW
    // WEAPON_GRAPPLING
    // WEAPON_FLARE
    WEAPON_MAX
};

struct WeaponParams
{
    ItemType modelType;
    ItemType animType;
    uint16 damage;
    uint16 spread;
    uint16 range;
    int16 height;
    int16 soundId;
    uint8 reloadTimer;
    uint8 flashOffset;
    uint8 flashTimer;
    uint8 flashIntensity;
    int16 aimX;
    int16 aimY;
    int16 armX;
    int16 armMinY;
    int16 armMaxY;
};

enum LaraArm
{
    LARA_ARM_R,
    LARA_ARM_L,
    LARA_ARM_MAX
};

enum LaraJoint
{
    JOINT_HIPS = 0,
    JOINT_LEG_L1,
    JOINT_LEG_L2,
    JOINT_LEG_L3,
    JOINT_LEG_R1,
    JOINT_LEG_R2,
    JOINT_LEG_R3,
    JOINT_TORSO,
    JOINT_ARM_R1,
    JOINT_ARM_R2,
    JOINT_ARM_R3,
    JOINT_ARM_L1,
    JOINT_ARM_L2,
    JOINT_ARM_L3,
    JOINT_HEAD,
    JOINT_MAX
};

struct ExtraInfoLara
{
    int16 swimTimer;
    uint8 weaponState;
    uint8 vSpeedHack;
    
    int16 moveAngle;
    int16 hitFrame;

    int8  hitTimer;
    int8  hitQuadrant;

    uint8 weapon;
    uint8 goalWeapon;

    struct Head {
        vec3s angle;
    } head;

    struct Torso {
        vec3s angle;
    } torso;

    struct Arm
    {
        vec3s angle;
        vec3s angleAim;

        uint16 animIndex;
        uint16 frameIndex;

        struct Flash {
            int16 timer;
            int16 angle;
            int16 offset;
            int16 intensity;
        } flash;

        ItemObj* target;

        bool aim;
        bool useBasis;
    };
    
    Arm armR;
    Arm armL;

    Camera camera;

    uint16 meshes[JOINT_MAX];

    uint16 ammo[WEAPON_MAX]; // TODO make global

    Nav nav;

    bool dozy;
};

extern ExtraInfoLara playersExtra[MAX_PLAYERS];

struct Enemy;

enum EnemyMood
{
    MOOD_SLEEP,
    MOOD_STALK,
    MOOD_ATTACK,
    MOOD_ESCAPE
};

struct ExtraInfoEnemy
{
    int16 rotHead;
    int16 rotNeck;

    int16 maxTurn;
    int16 _reserved;

    Enemy* enemy;

    Nav nav;
};

struct ItemObj
{
    Room* room;

    vec3i pos;
    vec3s angle;

    ItemObjFlags flags;

    int16 vSpeed;
    int16 hSpeed;

    union {
        uint16 animIndex;
        int16 tick; // effects only
    };

    uint16 frameIndex;

    uint8 state;
    uint8 nextState; // enemies only
    uint8 goalState;
    uint8 waterState;

    uint16 headOffset; // enemies only
    uint16 aggression; // enemies only

    int16 health;
    union {
        int16 timer;
        int16 oxygen; // Lara only
        int16 radius; // enemies only TODO
    };

    union {
        uint16 input; // Lara only
        uint16 mood; // enemies only
        int16 corpseTimer; // enemies only
    };
    int16 turnSpeed;

    uint8 type;
    uint8 intensity;
    int16 roomFloor;

    uint32 hitMask;
    uint32 visibleMask;

    union {
        uint8* extra;
        ExtraInfoLara* extraL;
        ExtraInfoEnemy* extraE;
    };

    ItemObj* nextItem;
    ItemObj* nextActive;

    static ItemObj* sFirstActive;
    static ItemObj* sFirstFree;

    static ItemObj* add(ItemType type, Room* room, const vec3i &pos, int32 angleY);
    void remove();

    void fxBubbles(Room *fxRoom, int32 fxJoint, const vec3i &fxOffset);
    void fxRicochet(Room *fxRoom, const vec3i &fxPos, bool fxSound);
    void fxBlood(const vec3i &fxPos, int16 fxAngleY, int16 fxSpeed);
    void fxSmoke(const vec3i &fxPos);
    void fxSplash();

    int32 getFrames(const AnimFrame* &frameA, const AnimFrame* &frameB, int32 &animFrameRate) const;
    const AnimFrame* getFrame() const;
    const AABBs& getBoundingBox(bool lerp) const;
    void move();
    const Anim* animSet(int32 newAnimIndex, bool resetState, int32 frameOffset = 0);
    const Anim* animChange(const Anim* anim);
    void animCmd(bool fx, const Anim* anim);
    void animSkip(int32 stateBefore, int32 stateAfter, bool advance = false);
    void animProcess(bool movement = true);
    bool animIsEnd(int32 offset) const;
    void animHit(int32 dirX, int32 dirZ, int32 hitTimer);
    bool moveTo(const vec3i &point, ItemObj* item, bool lerp);

    void updateRoom(int32 offset = 0);

    vec3i getRelative(const vec3i &point) const;

    int32 getWaterLevel() const;
    int32 getWaterDepth() const;
    int32 getBridgeFloor(int32 x, int32 z) const;
    int32 getTrapDoorFloor(int32 x, int32 z) const;
    int32 getDrawBridgeFloor(int32 x, int32 z) const;
    void getItemFloorCeiling(int32 x, int32 y, int32 z, int32* floor, int32* ceiling) const;

    vec3i getJoint(int32 jointIndex, const vec3i &offset) const;
    int32 getSpheres(Sphere* spheres, bool flag) const;

    uint32 collideSpheres(Lara* lara) const;
    bool collideBounds(Lara* lara, CollisionInfo* cinfo) const;
    void collidePush(Lara* lara, CollisionInfo* cinfo, bool enemyHit) const;
    void collideRoom(int32 height, int32 yOffset) const;

    uint32 updateHitMask(Lara* lara, CollisionInfo* cinfo);

    ItemObj* init(Room* room);

    X_INLINE ItemObj() {}
    ItemObj(Room* room);
    virtual void activate();
    virtual void deactivate();
    virtual void hit(int32 damage, const vec3i &point, int32 soundId);
    virtual void collide(Lara* lara, CollisionInfo* cinfo);
    virtual void update();
    virtual void draw();
};

struct SaveGame
{
    struct TrackFlags {
        uint8 once:1, mask:5, :2;
    };

    uint32 time;
    uint32 distance;
    uint32 secrets;
    uint32 pickups;
    uint32 mediUsed;
    uint32 ammoUsed;
    uint32 kills;
    TrackFlags tracks[64];
    uint32 flipped;
    int32 gamma;
};

struct Settings
{
    struct Controls {
        bool retarget;
    } controls;
};

#define FD_SET_END(x,end)   ((x) |= ((end) << 15))
#define FD_END(x)           ((x) >> 15)
#define FD_FLOOR_TYPE(x)    ((x) & 0x1F)
#define FD_TRIGGER_TYPE(x)  (((x) >> 8) & 0x7F)
#define FD_TIMER(x)         ((x) & 0xFF)
#define FD_ONCE(x)          (((x) >> 8) & 1)
#define FD_SPEED(x)         (((x) >> 9) & 0x1F)
#define FD_MASK(x)          (((x) >> 9) & 0x1F)
#define FD_ACTION(x)        (((x) >> 10) & 0x1F)
#define FD_ARGS(x)          ((x) & 0x03FF)
#define FD_SLANT_X(x)       int8((x) & 0xFF)
#define FD_SLANT_Z(x)       int8((x) >> 8)

typedef uint16 FloorData;

enum FloorType
{
    FLOOR_TYPE_NONE,
    FLOOR_TYPE_PORTAL,
    FLOOR_TYPE_FLOOR,
    FLOOR_TYPE_CEILING,
    FLOOR_TYPE_TRIGGER,
    FLOOR_TYPE_LAVA
};

enum TriggerType
{
    TRIGGER_TYPE_ACTIVATE,
    TRIGGER_TYPE_PAD,
    TRIGGER_TYPE_SWITCH,
    TRIGGER_TYPE_KEY,
    TRIGGER_TYPE_PICKUP,
    TRIGGER_TYPE_OBJECT,
    TRIGGER_TYPE_ANTIPAD,
    TRIGGER_TYPE_COMBAT,
    TRIGGER_TYPE_DUMMY
};

enum TriggerAction
{
    TRIGGER_ACTION_ACTIVATE_OBJECT,
    TRIGGER_ACTION_ACTIVATE_CAMERA,
    TRIGGER_ACTION_FLOW,
    TRIGGER_ACTION_FLIP,
    TRIGGER_ACTION_FLIP_ON,
    TRIGGER_ACTION_FLIP_OFF,
    TRIGGER_ACTION_CAMERA_TARGET,
    TRIGGER_ACTION_END,
    TRIGGER_ACTION_SOUNDTRACK,
    TRIGGER_ACTION_EFFECT,
    TRIGGER_ACTION_SECRET,
    TRIGGER_ACTION_CLEAR_BODIES,
    TRIGGER_ACTION_FLYBY,
    TRIGGER_ACTION_CUTSCENE
};

enum SlantType
{
    SLANT_NONE,
    SLANT_LOW,
    SLANT_HIGH
};

enum WaterState
{
    WATER_STATE_ABOVE,
    WATER_STATE_WADE,
    WATER_STATE_SURFACE,
    WATER_STATE_UNDER
};

enum AnimCommand
{
    ANIM_CMD_NONE,
    ANIM_CMD_OFFSET,
    ANIM_CMD_JUMP,
    ANIM_CMD_EMPTY,
    ANIM_CMD_KILL,
    ANIM_CMD_SOUND,
    ANIM_CMD_EFFECT
};

enum EffectType
{
    FX_NONE           = -1,
    FX_ROTATE_180     ,
    FX_FLOOR_SHAKE    ,
    FX_LARA_NORMAL    ,
    FX_LARA_BUBBLES   ,
    FX_FINISH_LEVEL   ,
    FX_EARTHQUAKE     ,
    FX_FLOOD          ,
    FX_UNK1           ,
    FX_STAIRS2SLOPE   ,
    FX_UNK3           ,
    FX_UNK4           ,
    FX_EXPLOSION      ,
    FX_LARA_HANDSFREE ,
    FX_FLIP_MAP       ,
    FX_DRAW_RIGHTGUN  ,
    FX_DRAW_LEFTGUN   ,
    FX_SHOT_RIGHTGUN  ,
    FX_SHOT_LEFTGUN   ,
    FX_MESH_SWAP_1    ,
    FX_MESH_SWAP_2    ,
    FX_MESH_SWAP_3    ,
    FX_INV_ON         ,
    FX_INV_OFF        ,
    FX_DYN_ON         ,
    FX_DYN_OFF        ,
    FX_STATUE_FX      ,
    FX_RESET_HAIR     ,
    FX_BOILER_FX      ,
    FX_ASSAULT_RESET  ,
    FX_ASSAULT_STOP   ,
    FX_ASSAULT_START  ,
    FX_ASSAULT_FINISH ,
    FX_FOOTPRINT      ,
// specific
    FX_TR1_FLICKER    = 16
};

enum SoundMode {
    UNIQUE,
    REPLAY,
    LOOP
};

enum SoundID
{
    SND_NO              = 2,

    SND_LANDING         = 4,
        
    SND_DRAW            = 6,
    SND_HOLSTER         = 7,
    SND_PISTOLS_SHOT    = 8,
    SND_SHOTGUN_RELOAD  = 9,
    SND_RICOCHET        = 10,
        
    SND_HIT_BEAR        = 16,
    SND_HIT_WOLF        = 20,

    SND_SCREAM          = 30,
    SND_HIT             = 27,
    SND_DAMAGE          = 31,

    SND_SPLASH          = 33,
        
    SND_BUBBLE          = 37,
         
    SND_UZIS_SHOT       = 43,
    SND_MAGNUMS_SHOT    = 44,
    SND_SHOTGUN_SHOT    = 45,
    SND_EMPTY           = 48,
    SND_HIT_UNDERWATER  = 50,
        
    SND_UNDERWATER      = 60,

    SND_BOULDER         = 70,

    SND_FLOOD           = 81,

    SND_HIT_LION        = 85,

    SND_HIT_RAT         = 95,
        
    SND_LIGHTNING       = 98,
    SND_ROCK            = 99,

    SND_SWORD           = 103,
    SND_EXPLOSION       = 104,

    SND_INV_SPIN        = 108,
    SND_INV_HOME        = 109,
    SND_INV_CONTROLS    = 110,
    SND_INV_SHOW        = 111,
    SND_INV_HIDE        = 112,
    SND_INV_COMPASS     = 113,
    SND_INV_WEAPON      = 114,
    SND_INV_PAGE        = 115,
    SND_HEALTH          = 116,
        
    SND_STAIRS2SLOPE    = 119,

    SND_NATLA_SHOT      = 123,

    SND_HIT_SKATER      = 132,

    SND_HIT_ADAM        = 142,
    SND_STOMP           = 147,
        
    SND_LAVA            = 149,
    SND_FLAME           = 150,
    SND_DART            = 151,
        
    SND_TNT             = 170,
    SND_MUTANT_DEATH    = 171,
    SND_SECRET          = 173,

    SND_HELICOPTER      = 297,

    SND_WINSTON_SCARED  = 344,
    SND_WINSTON_WALK    = 345,
    SND_WINSTON_PUSH    = 346,
    SND_WINSTON_TRAY    = 347
};

#define LARA_LOOK_ANGLE_MAX     ANGLE(22)
#define LARA_LOOK_ANGLE_MIN     ANGLE(-42)
#define LARA_LOOK_ANGLE_Y       ANGLE(44)
#define LARA_LOOK_TURN_SPEED    ANGLE(2)

enum CollisionType
{
    CT_NONE          = 0,
    CT_FRONT         = (1 << 0),
    CT_LEFT          = (1 << 1),
    CT_RIGHT         = (1 << 2),
    CT_CEILING       = (1 << 3),
    CT_FRONT_CEILING = (1 << 4),
    CT_FLOOR_CEILING = (1 << 5)
};

struct CollisionInfo
{
    enum SideType
    {
        ST_MIDDLE,
        ST_FRONT,
        ST_LEFT,
        ST_RIGHT,
        ST_MAX
    };

    struct Side
    {
        int32     floor;
        int32     ceiling;
        SlantType slantType;
    };

    const FloorData* trigger;

    Side m;
    Side f;
    Side l;
    Side r;

    int32 radius;

    int32 gapPos;
    int32 gapNeg;
    int32 gapCeiling;

    vec3i offset;
    vec3i pos;

    int16 angle;
    uint16 quadrant;

    CollisionType type;

    int8 slantX;
    int8 slantZ;

    bool enemyPush;
    bool enemyHit;
    bool staticHit;
    bool stopOnSlant;
    bool stopOnLava;

    void setSide(SideType st, int32 floor, int32 ceiling);

    X_INLINE void setAngle(int16 value)
    {
        angle = value;
        quadrant = uint16(value + ANGLE_45) >> ANGLE_SHIFT_90;
    }
};

struct Box
{
    int8 minZ, maxZ;
    int8 minX, maxX;
    int16 floor;
    uint16 overlap;
};

struct Level
{
    uint32 magic;

    uint16 tilesCount;
    uint16 roomsCount;
    uint16 modelsCount;
    uint16 meshesCount;
    uint16 staticMeshesCount;
    uint16 spriteSequencesCount;
    uint16 soundSourcesCount;
    uint16 boxesCount;
    uint16 texturesCount;
    uint16 itemsCount;
    uint16 camerasCount;
    uint16 cameraFramesCount;
    uint16 soundOffsetsCount;
    uint16 _reserved;

    const uint16* palette;
    const uint8* lightmap;
    const uint8* tiles;
    const RoomInfo* roomsInfo;
    const FloorData* floors;
    const Mesh** meshes;
    const int32* meshOffsets;
    const Anim* anims;
    const AnimState* animStates;
    const AnimRange* animRanges;
    const int16* animCommands;
    const ModelNode* nodes;
    const uint16* animFrames;
    const Model* models;
    const StaticMesh* staticMeshes;
    Texture* textures;
    const Sprite* sprites;
    const SpriteSeq* spriteSequences;
    FixedCamera* cameras;
    uint32 soundSources;
    Box* boxes;
    const uint16* overlaps;
    const uint16* zones[2][ZONE_MAX];
    const int16* animTexData;
    const ItemObjInfo* itemsInfo;
    uint32 cameraFrames;
    const uint16* soundMap;
    const SoundInfo* soundsInfo;
    const uint8* soundData;
    const int32* soundOffsets;
};

// used by enemies
struct TargetInfo
{
    ItemObj* target;
    vec3i waypoint;
    vec3i pos;
    int16 angle;
    int16 rotHead;
    int16 tilt;
    int16 turn;
    uint32 dist;
    uint16 boxIndex;
    uint16 boxIndexTarget;
    uint16 zoneIndex;
    uint16 zoneIndexTarget;
    bool aim;
    bool canAttack;
};

extern TargetInfo tinfo;

extern Level level;

struct IMA_STATE
{
    int32 smp;
    int32 idx;
};

/*
#define PERSPECTIVE(x, y, z) {\
    x = (x / (z >> 7));\
    y = (y / (z >> 6));\
}
*/
#if defined(MODEHW)
    #define PERSPECTIVE(x, y, z) {\
        x = (x << 6) / (z >> 2);\
        y = (y << 6) / (z >> 2);\
    }
#elif defined(MODE13)
    #define PERSPECTIVE(x, y, z) {\
        int32 dz = (z >> (FIXED_SHIFT + FOV_SHIFT - 1)) / 3;\
        if (dz >= DIV_TABLE_SIZE) dz = DIV_TABLE_SIZE - 1;\
        int32 d = FixedInvU(dz);\
        x = d * (x >> 14) >> 12;\
        y = d * (y >> 14) >> 12;\
    }
#elif defined(MODE4)
    #define PERSPECTIVE(x, y, z) {\
        int32 dz = (z >> (FIXED_SHIFT + FOV_SHIFT + 1));\
        dz += (dz >> 2);\
        ASSERT(dz < DIV_TABLE_SIZE);\
        int32 d = FixedInvU(dz);\
        x = d * (x >> FIXED_SHIFT) >> 12;\
        y = d * (y >> FIXED_SHIFT) >> 12;\
    }
#else
    #define PERSPECTIVE(x, y, z) {\
        int32 dz = (z >> (FIXED_SHIFT + FOV_SHIFT - 1)) / 3;\
        if (dz >= DIV_TABLE_SIZE) dz = DIV_TABLE_SIZE - 1;\
        int32 d = FixedInvU(dz);\
        x = d * (x >> FIXED_SHIFT) >> 13;\
        y = d * (y >> FIXED_SHIFT) >> 13;\
    }
#endif

// renderer internal
extern uint32 keys;
extern RectMinMax viewport;
extern vec3i cameraViewPos;
extern vec3i cameraViewOffset;
extern Matrix* matrixPtr;
extern Matrix matrixStack[MAX_MATRICES];
extern int32 gVerticesCount;
extern int32 gFacesCount;

#ifndef MODEHW
    extern AABBi frustumAABB;
#endif

extern SaveGame gSaveGame;
extern Settings gSettings;
extern int32 gCurTrack;

extern const FloorData* gLastFloorData;
extern FloorData gLastFloorSlant;

extern Room rooms[MAX_ROOMS];
extern ItemObj items[MAX_ITEMS];

// level data
extern bool enableClipping;

template <class T>
X_INLINE void swap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

void set_seed_logic(int32 seed);
void set_seed_draw(int32 seed);
int32 rand_logic();
int32 rand_draw();

#define RAND_LOGIC(r) (rand_logic() * (r) >> 15)
#define RAND_DRAW(r)  (rand_draw() * (r) >> 15)

int32 phd_sin(int32 x);
int32 phd_cos(int32 x);
int32 phd_atan(int32 x, int32 y);
uint32 phd_sqrt(uint32 x);

X_INLINE int32 dot(const vec3i &a, const vec3i &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

X_INLINE int32 fastLength(int32 dx, int32 dy)
{
    dx = abs(dx);
    dy = abs(dy);
    return (dx > dy) ? (dx + (dy >> 1)) : (dy + (dx >> 1));
}

void anglesFromVector(int32 x, int32 y, int32 z, int16 &angleX, int16 &angleY);

X_INLINE int16 angleLerp(int16 a, int16 b, int32 w)
{
    int16 d = b - a;
    if (d > +w) return a + w;
    if (d < -w) return a - w;
    return b;
}

#define angleDec(angle, value) angleLerp(angle, 0, value)

AABBs boxRotate(const AABBs &box, int16 angle);
void boxTranslate(AABBs &box, const vec3i &offset);
bool boxIntersect(const AABBs &a, const AABBs &b);
bool boxContains(const AABBs &a, const vec3i &p);
vec3i boxPushOut(const AABBs &a, const AABBs &b);

#define DECODE_ANGLES(a,x,y,z)\
    x = (((uint16*)(a))[1] & 0x3FF0) << 2;\
    y = (((uint16*)(a))[1] & 0x000F) << 12 | (((uint16*)(a))[0] & 0xFC00) >> 4;\
    z = (((uint16*)(a))[0] & 0x03FF) << 6;

#define LERP_1_2(a, b, mul, div) a = (b + a) >> 1
#define LERP_1_3(a, b, mul, div) a = a + ((b - a) / 3)
#define LERP_2_3(a, b, mul, div) a = b - ((b - a) / 3)
#define LERP_1_4(a, b, mul, div) a = a + ((b - a) >> 2)
#define LERP_3_4(a, b, mul, div) a = b - ((b - a) >> 2)
#define LERP_1_5(a, b, mul, div) a = a + ((b - a) / 5)
#define LERP_2_5(a, b, mul, div) a = a + ((b - a) * 2 / 5)
#define LERP_3_5(a, b, mul, div) a = b - ((b - a) * 2 / 5)
#define LERP_4_5(a, b, mul, div) a = b - ((b - a) / 5)
#define LERP_SLOW(a, b, mul, div) a = a + ((b - a) * mul / div)

#define LERP_ROW(lerp_func, a, b, mul, div) \
    lerp_func(a##0, b##0, mul, div); \
    lerp_func(a##1, b##1, mul, div); \
    lerp_func(a##2, b##2, mul, div); \
    lerp_func(a##3, b##3, mul, div);

#define LERP_MATRIX(lerp_func) \
    LERP_ROW(lerp_func, m.e0, n.e0, multiplier, divider); \
    LERP_ROW(lerp_func, m.e1, n.e1, multiplier, divider); \
    LERP_ROW(lerp_func, m.e2, n.e2, multiplier, divider);

X_INLINE Matrix& matrixGet()
{
    return *matrixPtr;
}

X_INLINE void matrixPush()
{
    memcpy(matrixPtr + 1, matrixPtr, sizeof(Matrix));
    matrixPtr++;
}

X_INLINE void matrixPop()
{
    matrixPtr--;
}

X_INLINE void matrixSetBasis(Matrix &dst, const Matrix &src)
{
    dst.e00 = src.e00;
    dst.e01 = src.e01;
    dst.e02 = src.e02;

    dst.e10 = src.e10;
    dst.e11 = src.e11;
    dst.e12 = src.e12;

    dst.e10 = src.e10;
    dst.e11 = src.e11;
    dst.e12 = src.e12;
}

X_INLINE vec3i matrixGetDir(const Matrix &m)
{
    return _vec3i(m.e20, m.e21, m.e22);
}

void matrixTranslate(int32 x, int32 y, int32 z);
void matrixTranslateAbs(int32 x, int32 y, int32 z);
void matrixRotateX(int32 angle);
void matrixRotateY(int32 angle);
void matrixRotateZ(int32 angle);
void matrixRotateYXZ(int32 angleX, int32 angleY, int32 angleZ);
void matrixRotateZXY(int32 angleX, int32 angleY, int32 angleZ);
void matrixLerp(const Matrix &n, int32 multiplier, int32 divider);
void matrixFrame(const vec3s &pos, const uint32* angles);
void matrixFrameLerp(const vec3s &pos, const uint32* anglesA, const uint32* anglesB, int32 delta, int32 rate);
void matrixSetIdentity();
void matrixSetView(const vec3i &pos, int32 angleX, int32 angleY);

void setGamma(int32 value);
void drawGlyph(const Sprite *sprite, int32 x, int32 y);

void renderInit();
void setViewport(const RectMinMax &vp);
void setPaletteIndex(int32 index);
void clear();
int32 rectIsVisible(const RectMinMax* rect);
int32 boxIsVisible(const AABBs* box);
void transform(vec3i* points, int32 count);
bool transformBoxRect(const AABBs* box, RectMinMax* rect);
void transformRoom(const RoomVertex* vertices, int32 vCount, bool underwater);
void transformMesh(const MeshVertex* vertices, int32 vCount, const uint16* vIntensity, const vec3s* vNormal);
void faceAddQuad(uint32 flags, const Index* indices);
void faceAddTriangle(uint32 flags, const Index* indices);
void faceAddShadow(int32 x, int32 z, int32 sx, int32 sz);
void faceAddSprite(int32 vx, int32 vy, int32 vz, int32 vg, int32 index);
void faceAddGlyph(int32 vx, int32 vy, int32 index);
void faceAddRoom(const RoomQuad* quads, int32 qCount, const RoomTriangle* triangles, int32 tCount);
void faceAddMesh(const MeshQuad* rFaces, const MeshQuad* crFaces, const MeshTriangle* tFaces, const MeshTriangle* ctFaces, int32 rCount, int32 crCount, int32 tCount, int32 ctCount);
void flush();

void drawInit();
void drawFree();
void drawModel(const ItemObj* item);
void drawItem(const ItemObj* item);
void drawRooms(Camera* camera);

void checkTrigger(const FloorData* fd, ItemObj* lara);
void readLevel(const uint8 *data);
bool trace(const Location &from, Location &to, bool accurate);

Lara* getLara(const vec3i &pos);

bool useSwitch(ItemObj* item, int32 timer);
bool useKey(ItemObj* item, ItemObj* lara);
bool usePickup(ItemObj* item);

void nextLevel();

int32 doTutorial(ItemObj* lara, int32 track);

void sndInit();
void sndInitSamples();
void sndFreeSamples();
void sndFill(uint8* buffer, int32 count);
void* sndPlaySample(int32 index, int32 volume, int32 pitch, int32 mode);
void sndPlayTrack(int32 track);
bool sndTrackIsPlaying();
void sndStopTrack();
void sndStopSample(int32 index);
void sndStop();

X_INLINE void dmaFill(void *dst, uint8 value, uint32 count)
{
    ASSERT((count & 3) == 0);
#ifdef __GBA__
    vu32 v = value;
    dma3_fill(dst, v, count);
#else
    memset(dst, value, count);
#endif
}

#endif