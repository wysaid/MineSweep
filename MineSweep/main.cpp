/*

 * author      wysaid
 * E-mail      admin@wysaid.com  OR  wysaid@gmail.com
 * date        20110110

*/

#include "MineSweep.h"

BlockType MARK;
MineArray g_mineArray;
BGManager g_bgm;
HWND g_hwnd;
int g_scrWidth = DEFAULT_MINE_WIDTH * IMG_SIZE, g_scrHeight = DEFAULT_MINE_HEIGHT * IMG_SIZE;
int g_minNum = 50;

void startFrame()
{
    g_bgm.playStart();
    setcolor(YELLOW);
    setfont(22, 11, "楷体");
    for (int i = 0; i < g_scrWidth; i += 35)
    {
        for (int j = 0; j < g_scrHeight; j += 35)
        {
            putimage(i, j, Block::imgBlock[0]);
        }
    }

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = g_scrWidth;
    rect.bottom = g_scrHeight;
    outtextrect(0, 0, g_scrWidth, g_scrHeight, "点击任意方格以开始游戏! \n 注意:\n游戏开始后按住ESC点击或者移动鼠标可退出,\n按住space键点击或者移动鼠标可以激活输入窗口自由设定雷区大小\n和雷数! \n 按住Enter键点击或者移动鼠标可以重新开始\n扫雷游戏的规则请自行百度.\n提示：右键插旗，左右双键可以探测快速扫雷。\n第一次点击以后初始化，肯定不会踩到雷");
    Sleep(1);
}

inline Message translateMsg(MOUSEMSG& msg)
{
    static bool isSearching = false;
    switch (msg.uMsg)
    {
    case WM_LBUTTONDOWN:
        if (msg.mkRButton)
        {
            isSearching = true;
            return msgSEARCH;
        }
        break;
    case WM_RBUTTONDOWN:
        if (msg.mkLButton)
        {
            isSearching = true;
            return msgSEARCH;
        }
        else
            return msgGUESS;
    case WM_LBUTTONUP:
        isSearching = false;
        if (msg.mkRButton) return msgENDSEARCH;
        if (isSearching)
        {
            isSearching = false;
            return msgMOVE;
        }
        return msgSWEEP;
    case WM_RBUTTONUP:
        if (msg.mkLButton) return msgENDSEARCH;
        isSearching = false;
    default:;
    }
    return msgMOVE;
}

inline void keyDownFrame(int x, int y)
{
    putimage(x / IMG_SIZE * IMG_SIZE, y / IMG_SIZE * IMG_SIZE, Block::imgSpace[0]);
}

DIRECTIONS play()
{
    MOUSEMSG msg;
    Message click;
    static int lastx, lasty;
    bool win = false;
    bool isKeyDown = false, isSearching = false;
    setcolor(BLACK);
    setfont(35, 35, "宋体");
    g_mineArray.render();

    if (g_mineArray.win())
        return dirWIN;
    for (; is_run(); delay_fps(60))
    {
        while (kbhit())
        {
            char ch = getch();
            if (ch == 27)
                return dirEXIT;
            else if (ch == 32)
                return dirSET;
            else if (ch == '\r')
                return dirREPLAY;
            if (msg.x < 0 || msg.x > g_scrWidth || msg.y < 0 || msg.y > g_scrHeight)
                continue;
        }
        bool needRedraw = false;
        while (mousemsg())
        {
            msg = GetMouseMsg();
            if (msg.uMsg == WM_MOUSEMOVE && msg.x / IMG_SIZE == lastx / IMG_SIZE && msg.y / IMG_SIZE == lasty / IMG_SIZE)
                continue;
            needRedraw = true;
            lastx = msg.x;
            lasty = msg.y;
            click = translateMsg(msg);
            switch (click)
            {
            case msgSWEEP:
                if (!g_mineArray.sweep(msg.x, msg.y))
                {
                    g_bgm.playBomb();
                    return dirLOSE;
                }
                else
                {
                    g_bgm.playClick();
                }
                if (g_mineArray.win())
                    return dirWIN;
                break;
            case msgGUESS:
                g_bgm.playRightClick();
                g_mineArray.guess(msg.x, msg.y);
                break;
            case msgSEARCH:
                isSearching = true;
                break;
            case msgENDSEARCH:
                isSearching = false;
                if (!g_mineArray.search(msg.x, msg.y))
                {
                    g_bgm.playBomb();
                    return dirLOSE;
                }
                else
                {
                    g_bgm.playSearch();
                }
                if (g_mineArray.win())
                    return dirWIN;
                break;

            default:;
            }
        }
        if (needRedraw)
        {
            cleardevice();
            g_mineArray.render();
            if (isSearching)
            {
                g_mineArray.searchFrame(msg.x, msg.y);
            }
            else if (msg.mkLButton)
            {
                keyDownFrame(msg.x, msg.y);
            }
            else if (click == msgMOVE)
            {
                g_bgm.playMove();
                g_mineArray.moveFrame(msg.x, msg.y);
            }
        }
    }

    return dirWIN;
}

void replay()
{
    void set();
    startFrame();
    MOUSEMSG msg;

    Sleep(0);

    for (; is_run(); delay_fps(60))
    {
        if (mousemsg() && (msg = GetMouseMsg()).uMsg == WM_LBUTTONUP)
            break;
        if (kbhit() && getch() == ' ')
        {
            set();
            return;
        }
    }
    int x = g_scrWidth / IMG_SIZE, y = g_scrHeight / IMG_SIZE;
    g_mineArray.reInit(x, y, msg.x, msg.y, g_minNum);
    g_mineArray.sweep(msg.x, msg.y);
    flushmouse();
    while (kbhit())
        getch();
}

void set()
{
    char str[64];
    const char* p = "请输入所需的列数(width)、行数(height)以及地雷数，用空格隔开";
    int w, h, n;
    for (bool b = false; !b;)
    {
        b = !!inputbox_getline("示例: 20 15 50", p, str, 64);
        sscanf(str, "%d%d%d", &w, &h, &n);
        if (w < 1 || w > GetSystemMetrics(SM_CXSCREEN) / IMG_SIZE)
        {
            b = false;
            p = "输入的列数不合法!请重新输入!请输入所需的列数(width)、行数(height)以及地雷数，用空格隔开";
        }
        else if (h < 1 || h > GetSystemMetrics(SM_CYSCREEN) / IMG_SIZE)
        {
            p = "输入的行数不合法!请重新输入!请输入所需的列数(width)、行数(height)以及地雷数，用空格隔开";
            b = false;
        }
        else if (n >= w * h - 1)
        {
            p = "输入的地雷数过多!请重新输入!请输入所需的列数(width)、行数(height)以及地雷数，用空格隔开";
            b = false;
        }
    }
    g_scrWidth = w * IMG_SIZE;
    g_scrHeight = h * IMG_SIZE;
    g_minNum = n;

    initgraph(g_scrWidth, g_scrHeight);

    setbkmode(TRANSPARENT);
    g_hwnd = getHWnd();
    SetWindowText(g_hwnd, "MineSweep —— by wysaid");
    flushmouse();
    replay();
}

void youWin()
{
    g_bgm.playWin();
    g_mineArray.winFrame();
    cleardevice();
    setcolor(YELLOW);
    setfont(30, 15, "宋体");
    outtextxy(20, g_scrHeight / 2, "游戏将在1.5秒后开始初始化并重新开始");

    flushmouse();
    Sleep(1500);
    replay();
}

void youLose()
{
    g_mineArray.loseFrame();
    g_bgm.cleanBomb();
    cleardevice();
    setcolor(YELLOW);
    setfont(30, 15, "宋体");
    outtextxy(20, g_scrHeight / 2, "游戏将在1.5秒后初始化完毕并重新开始");

    flushmouse();
    Sleep(1500);
    replay();
}

void init()
{
    initgraph(g_scrWidth, g_scrHeight);
    setbkmode(TRANSPARENT);
    g_hwnd = getHWnd();
    SetWindowText(g_hwnd, "Simple MineSweep —— by wysaid");
    g_mineArray.loadResource();
    startFrame();
    MOUSEMSG msg;
    for (; is_run(); delay_fps(60))
    {
        if (mousemsg() && (msg = GetMouseMsg()).uMsg == WM_LBUTTONUP)
            break;
        if (kbhit() && getch() == ' ')
        {
            set();
            return;
        }
    }
    g_mineArray.init(DEFAULT_MINE_WIDTH, DEFAULT_MINE_HEIGHT, msg.x, msg.y, g_minNum);
    g_mineArray.sweep(msg.x, msg.y);
    g_bgm.playClick();
}

int main()
{
    init();
    while (1)
    {
        switch (play())
        {
        case dirREPLAY:
            replay();
            break;
        case dirSET:
            set();
            break;
        case dirEXIT:
            cleardevice();
            setcolor(RED);
            setfont(50, 25, "宋体");
            outtextxy(50, g_scrHeight / 2, "wysaid: 欢迎使用，再见~~");
            outtextxy(50, g_scrHeight / 2 + 100, "程序将在两秒钟后安全退出~~~~");
            Sleep(2000);
            closegraph();
            return 0;
        case dirWIN:
            youWin();
            break;
        case dirLOSE:
            youLose();
            break;
        default:;
        }
    }
    return 0;
}
