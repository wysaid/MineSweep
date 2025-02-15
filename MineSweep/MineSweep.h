/*

 * author      wysaid
 * E-mail      admin@wysaid.com  OR  wysaid@gmail.com
 * date        20110110

*/

#ifndef _MINESWEEP_H_
#define _MINESWEEP_H_

#ifdef UNICODE
#undef UNICODE
#endif

#pragma warning(disable : 4996)

#include "graphics.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

#pragma comment(lib, "Winmm.lib")

#define IMG_SIZE 35
#define DEFAULT_MINE_WIDTH 20
#define DEFAULT_MINE_HEIGHT 15

#define BUFFERSIZE 1024

#define PIXEL2MAP(x, y)          \
    x = x / IMG_SIZE * IMG_SIZE; \
    y = y / IMG_SIZE * IMG_SIZE
#define PIXEL2BLOCK(x, y) \
    x /= IMG_SIZE;        \
    y /= IMG_SIZE
#define DELAY_FPS (60 / 7)

#define RANDF ((rand() % 1000 / 1000.0 + 0.2) * pow(-1.0f, rand() % 10))

enum BlockType
{
    NORMAL,
    CHOSEN,
    FLAG,
    FLAG_CHOSEN,
    FLAG_MINE,
    WRONGFLAG,
    UNKNOWN,
    UNKNOWN_CHOSEN,
    WRONGQMARK,
    SPACE,
    UNKNOWN_MINE,
    ONFIRE,
    FOUND,
    MARKEDMINE,
    MINE
};
enum Message
{
    msgSWEEP,
    msgGUESS,
    msgSEARCH,
    msgENDSEARCH,
    msgKEYDOWN,
    msgKEYUP,
    msgMOVE
};
enum DIRECTIONS
{
    dirREPLAY,
    dirSET,
    dirEXIT,
    dirWIN,
    dirLOSE
};

class BGManager
{
public:
    BGManager();
    ~BGManager();

    void playStart();
    void playClick();
    void playRightClick();
    void playSearch();
    void playMove();
    void playBomb();
    void playWin();

    void cleanBomb();

private:
    int m_bombCnt;
    const char* m_bomb;
};

class Block
{
public:
    Block() :
        m_x(0), m_y(0), m_type(NORMAL), m_num(0) /*, isSwept(false)*/ {}
    Block(int x, int y) :
        m_x(x), m_y(y), m_type(NORMAL), m_num(0) {}
    virtual void render();
    virtual bool isMine() { return false; }
    virtual void addMine() { ++m_num; }
    virtual int getNum() { return m_num; }
    virtual void setFire() {}
    void getPos(int& x, int& y)
    {
        x = m_x;
        y = m_y;
    }
    BlockType getType() { return m_type; }
    void setType(BlockType type) { m_type = type; }
    bool canBeSearched()
    {
        return m_type == FOUND || m_type == FLAG ? false : true;
    }
    static PIMAGE *imgBlock, *imgMine, *imgFlag, *imgUnknown, *imgSpace;

protected:
    int m_x, m_y;
    BlockType m_type;

private:
    int m_num;
};

class Mine : public Block
{
public:
    Mine() :
        m_frame(0), m_f(0), m_dx(0.0f), m_dy(0.0f), m_mx(0.0f), m_my(0.0f) {}
    Mine(int x, int y);
    void render();
    bool isMine() { return true; }
    void addMine() {}
    void setFire();
    int getNum() { return -1; }
    static PIMAGE *imgBlock, *imgMine, *imgFlag, *imgUnknown, *imgSpace;
    static int bomb;

private:
    void frameUpdate();
    void go();
    float m_dx, m_dy;
    float m_mx, m_my;
    int m_frame, m_f;
};

class MineArray
{
public:
    MineArray() :
        m_width(0), m_height(0), m_firedX(0), m_firedY(0), m_keyCtrl(KEYFREE), m_block(NULL), m_imgBlock(NULL), m_imgMine(NULL), m_imgFlag(NULL), m_imgUnknown(NULL), m_imgSpace(NULL), m_blockNum(0) {}

    enum KeyState
    {
        KEYDOWN,
        KEYFREE,
        SEARCHING
    };

    void loadResource();
    bool init(int width, int height, int x, int y, int num);
    bool reInit(int width, int height, int x, int y, int num);
    bool win() { return m_blockNum <= 0; }
    bool sweep(int x, int y);
    void keyDown(int, int);
    void guess(int, int);
    void moveFrame(int, int);
    void searchFrame(int x, int y);
    bool search(int, int);
    void render();
    void winFrame();
    void loseFrame();
    ~MineArray();

private:
    void randomize(bool** b, int width, int height, int num, int, int);
    void initBlocks(bool**);
    void findMore(int, int);
    void markAll();
    int m_width, m_height;
    int m_firedX, m_firedY;
    int m_blockNum;
    KeyState m_keyCtrl;
    Block*** m_block;
    PIMAGE *m_imgBlock, *m_imgMine, *m_imgFlag, *m_imgUnknown, *m_imgSpace;
};

#endif