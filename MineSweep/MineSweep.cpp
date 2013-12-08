/*

 * author      wysaid
 * E-mail      admin@wysaid.org  OR  wysaid@gmail.com
 * date        20110110

*/

#include "MineSweep.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

extern BlockType MARK;
extern int g_scrWidth, g_scrHeight;
extern BGManager g_bgm;
PIMAGE *Block::imgBlock, *Block::imgMine, *Block::imgFlag, *Block::imgUnknown, *Block::imgSpace;
PIMAGE *Mine::imgBlock, *Mine::imgMine, *Mine::imgFlag, *Mine::imgUnknown, *Mine::imgSpace;
int Mine::bomb = 0;

BGManager::BGManager() : m_bombCnt(0)
{
	mciSendString("open start.mp3 alias startAudio", NULL, 0, NULL);
	mciSendString("open click.wav alias clickAudio", NULL, 0, NULL);
	mciSendString("open rightClick.wav alias rightClickAudio", NULL, 0, NULL);
	mciSendString("open search.wav alias searchAudio", NULL, 0, NULL);
	mciSendString("open move.wav alias moveAudio", NULL, 0, NULL);
	mciSendString("open win.mp3 alias winAudio", NULL, 0, NULL);

	m_bomb = "bomb.wav";
}

BGManager::~BGManager()
{
	mciSendString("close startAudio", NULL, 0, NULL);
	mciSendString("close clickAudio", NULL, 0, NULL);
	mciSendString("close rightClickAudio", NULL, 0, NULL);
	mciSendString("close searchAudio", NULL, 0, NULL);
	mciSendString("close moveAudio", NULL, 0, NULL);
	mciSendString("close winAudio", NULL, 0, NULL);

	for(int i = 0; i < m_bombCnt; ++i)
	{
		char buffer[BUFFERSIZE];
		sprintf(buffer, "close %s", m_bomb);
		mciSendString(buffer, NULL, 0, NULL);
	}

}

void BGManager::playStart()
{
	mciSendString("seek startAudio to 0", NULL, 0, NULL);
	mciSendString("play startAudio", NULL, 0, NULL);
}

void BGManager::playClick()
{
	mciSendString("seek clickAudio to 0", NULL, 0, NULL);
	mciSendString("play clickAudio", NULL, 0, NULL);
}

void BGManager::playRightClick()
{
	mciSendString("seek rightClickAudio to 0", NULL, 0, NULL);
	mciSendString("play rightClickAudio", NULL, 0, NULL);
}

void BGManager::playSearch()
{
	mciSendString("seek searchAudio to 0", NULL, 0, NULL);
	mciSendString("play searchAudio", NULL, 0, NULL);
}

void BGManager::playMove()
{
// 	mciSendString("seek moveAudio to 0", NULL, 0, NULL);
// 	mciSendString("play moveAudio", NULL, 0, NULL);
}

void BGManager::playWin()
{
	mciSendString("seek winAudio to 0", NULL, 0, NULL);
	mciSendString("play winAudio", NULL, 0, NULL);
}

void BGManager::cleanBomb()
{
	for(int i = 0; i != m_bombCnt; ++i)
	{
		char buffer[BUFFERSIZE];
		sprintf(buffer, "close bombAudio%d", i);
		mciSendString(buffer, NULL, 0, NULL);
	}
	m_bombCnt = 0;
}

void BGManager::playBomb()
{
	char buffer[BUFFERSIZE];
	sprintf(buffer, "open %s alias bombAudio%d", m_bomb, m_bombCnt);
	mciSendString(buffer, NULL, 0, NULL);
	sprintf(buffer, "play bombAudio%d", m_bombCnt++);
	mciSendString(buffer, NULL, 0, NULL);
}

void Block::render()
{
	switch(m_type)
	{
	case NORMAL: putimage(m_x, m_y,imgBlock[0]); break;
	case CHOSEN: putimage(m_x, m_y,imgBlock[1]); break;
	case FLAG: putimage(m_x, m_y,imgFlag[0]); break;
	case FLAG_CHOSEN: putimage(m_x, m_y,imgFlag[1]); break;
	case UNKNOWN: putimage(m_x, m_y,imgUnknown[0]); break;
	case UNKNOWN_CHOSEN: putimage(m_x, m_y,imgUnknown[1]); break;
	case WRONGFLAG: putimage(m_x, m_y,imgMine[8]); break;
	case SPACE:
	case FOUND: 
		putimage(m_x, m_y, imgSpace[0]); 
		if(m_num != 0)
		{
			char s[64];
			sprintf(s, "%d", m_num);
			outtextxy(m_x, m_y, s);
		}
		break;
	default:;
	}
}

Mine::Mine(int x, int y) : Block(x, y), m_frame(0), m_f(0)
{
	m_dx = RANDF;
	m_dy = RANDF;
	m_mx = m_x;
	m_my = m_y;
}

void Mine::m_frameUpdate()
{
	if(m_frame >= 7)
		setType(MINE);
	if(++m_f > DELAY_FPS)
	{
		++m_frame %= 8;
		m_f = 0;
	}
}

void Mine::m_go()
{
	m_mx += m_dx;
	m_my += m_dy;
	m_x = m_mx;
	m_y = m_my;
	if(m_mx < 0 || m_mx > g_scrWidth-10)
	{
		m_dx = -m_dx;
		m_x += m_dx;
	}
	if(m_my < 0 || m_my > g_scrHeight-10)
	{
		m_dy = -m_dy;
		m_y += m_dy;
	}
}

void Mine::setFire()
{
// 	m_dx = (randomf() + 0.4) * pow(-1, random(10));
// 	m_dy = (randomf() + 0.4) * pow(-1, random(10));
}

void Mine::render()
{
	switch(m_type)
	{
	case NORMAL: putimage(m_x, m_y,imgBlock[0]); break;
	case CHOSEN: putimage(m_x, m_y,imgBlock[0]); break;
	case FLAG: putimage(m_x, m_y,imgFlag[0]); break;
	case FLAG_CHOSEN: putimage(m_x, m_y,imgFlag[0]); break;
	case UNKNOWN: putimage(m_x, m_y,imgUnknown[0]); break;
	case UNKNOWN_CHOSEN: putimage(m_x, m_y,imgUnknown[0]); break;
	case FLAG_MINE: putimage(m_x, m_y,imgFlag[0]); break;
	case UNKNOWN_MINE: putimage(m_x, m_y,imgUnknown[0]); break;
	case SPACE: putimage(m_x, m_y,imgSpace[0]); break;
	case MARKEDMINE:
		putimage(m_x, m_y,imgMine[0]); break;
		break;
	case MINE: 
		m_go();
		break;
	case ONFIRE: 
		{
			putimage(m_x, m_y,imgMine[m_frame]);
			m_frameUpdate(); 
		}
		break;
	default:;
	}
}

bool MineArray::reInit(int width, int height, int x, int y, int num)
{
	for(int a = 0; a != m_width; ++a)
	{
		for(int b = 0; b != m_height; ++b)
		{
			delete m_block[a][b];
		}
		delete[] m_block[a];
	}
	delete[] m_block;
	
	return init(width, height, x, y, num);
}

bool MineArray::init(int width, int height, int x, int y, int num)
{
	x /= IMG_SIZE;
	y /= IMG_SIZE;
	m_width = width;
	m_height = height;
	m_blockNum = width * height - num;
	if(num >= width * height) return false;
	int i;
	bool** mark = new bool*[width];
	for(i = 0; i != width; ++i)
	{
		mark[i] = new bool[height];
		memset(mark[i], 0, height * sizeof(bool));
	}
	m_randomize(mark, width, height, num, x, y);
	
	m_block = new Block**[width];
	for(i = 0; i != width; ++i)
	{
		m_block[i] = new Block*[height];
	}
	
	for(int a = 0; a != width; ++a)
	{	for(int b = 0; b != height; ++b)
		{
			if(mark[a][b]) 
				m_block[a][b] = new Mine(a * IMG_SIZE, b * IMG_SIZE);
			else 
				m_block[a][b] = new Block(a * IMG_SIZE, b * IMG_SIZE);
		}
	}
	m_initBlocks(mark);
	for(i = 0; i != width; ++i)
	{
		delete[] mark[i];
	}
	delete[] mark;
	
	return true;
}

void MineArray::m_initBlocks(bool** mark)
{
	for(int a = 0; a != m_width; ++a)
	{
		for(int b = 0; b != m_height; ++b)
		{
			if(mark[a][b])
			{
				if(a-1 >= 0) m_block[a-1][b]->addMine();				
				if(b-1 >= 0) m_block[a][b-1]->addMine();
				if(a-1 >= 0 && b-1 >= 0) m_block[a-1][b-1]->addMine();
				if(a+1 < m_width) m_block[a+1][b]->addMine();
				if(b+1 < m_height) m_block[a][b+1]->addMine();
				if(a+1 < m_width && b+1 < m_height) m_block[a+1][b+1]->addMine();
				if(a-1 >=0 && b+1 < m_height) m_block[a-1][b+1]->addMine();
				if(a+1 < m_width && b-1 >= 0) m_block[a+1][b-1]->addMine();
			}
		}
	}
}

MineArray::	~MineArray()
{
	for(int a = 0; a != m_width; ++a)
	{
		for(int b = 0; b != m_height; ++b)
		{
			delete m_block[a][b];
		}
		delete[] m_block[a];
	}
	delete[] m_block;
	
	for(int i = 0; i != 2; ++i)
		delimage(m_imgBlock[i]);
	for(int i = 0; i != 9; ++i)
		delimage(m_imgMine[i]);
	for(int i = 0; i != 3; ++i)
	{
		delimage(m_imgFlag[i]);
		delimage(m_imgUnknown[i]);
	}
	delimage(m_imgSpace[0]);
}

void MineArray::m_randomize(bool** b, int width, int height, int num, int x, int y)
{
	int rndx, rndy, cnt = 0, length = width * height, firstclick = x*width + height;
	srand(unsigned(time(NULL)));
	while(num > 0)
	{
		rndx = rand() % width;
		rndy = rand() % height;
		if(rndx == x && rndy == y) continue;
		if(b[rndx][rndy])
		{
			if(++cnt > 10)
			{
				int i;
				for(i = 0; i != length; ++i)
				{
					if(!b[i/width][i%height] && i != firstclick)
					{
						b[i/width][i%height] = true;
						cnt = 0;
						break;
					}
				}
				if(i == length) return;
			}
		}
		else
		{
			b[rndx][rndy] = true;
			--num;
			cnt = 0;
		}
	}
}

void MineArray::loadResource()
{
	m_imgBlock = new PIMAGE[2];
	m_imgMine = new PIMAGE[9];
	m_imgFlag = new PIMAGE[3];
	m_imgUnknown = new PIMAGE[3];
	m_imgSpace = new PIMAGE[1];
	
	m_imgBlock[0] = newimage();
	m_imgBlock[1] = newimage();

	getimage(m_imgBlock[0],"block_normal.bmp");
 	getimage(m_imgBlock[1],"block_chosen.bmp");
	
	for(int i=0; i != 9; ++i)
	{
		char buf[256];
		sprintf(buf, "mine_%d.bmp", i);
		m_imgMine[i] = newimage();
		getimage(m_imgMine[i],buf);
	}
	m_imgFlag[0] = newimage();
	m_imgFlag[1] = newimage();
	m_imgFlag[2] = newimage();

	getimage(m_imgFlag[0],"flag.bmp");
	getimage(m_imgFlag[1],"flag_chosen.bmp");
	getimage(m_imgFlag[2],"flag_mine.bmp");


	m_imgUnknown[0] = newimage();
	m_imgUnknown[1] = newimage();
	m_imgUnknown[2] = newimage();

	getimage(m_imgUnknown[0], "quote.bmp");
	getimage(m_imgUnknown[1], "quote_chosen.bmp");
	getimage(m_imgUnknown[2], "quote_mine.bmp");

	m_imgSpace[0] = newimage();

	getimage(m_imgSpace[0],"space.bmp");
	
	Block::imgBlock = m_imgBlock;
	Block::imgMine = m_imgMine;
	Block::imgFlag = m_imgFlag;
	Block::imgUnknown = m_imgUnknown;
	Block::imgSpace = m_imgSpace;

	Mine::imgBlock = m_imgBlock;
	Mine::imgMine = m_imgMine;
	Mine::imgFlag = m_imgFlag;
	Mine::imgUnknown = m_imgUnknown;
	Mine::imgSpace = m_imgSpace;
}

void MineArray::render()
{
	for(int i=0; i < m_width; ++i)
	{
		for(int j=0; j < m_height; ++j)
			m_block[i][j]->render();
	}
}

// void MineArray::frameUpdate()
// {
// 	for(int i=0; i < g_scrWidth; i += IMG_SIZE);
// 	{
// 		for(int j=0; j < g_scrHeight; j += IMG_SIZE);
// 			m_block[i][j]->frameUpdate();
// 	}
// }

void MineArray::searchFrame(int x, int y)
{
	PIXEL2BLOCK(x,y);
	for(int i = x - 1; i <= x + 1; ++i)
		for(int j = y- 1; j <= y + 1; ++j)
		{
			if(i >= 0 && i < m_width && j >= 0 && j < m_height && m_block[i][j]->canBeSearched())
			{
				putimage(i * IMG_SIZE, j * IMG_SIZE, m_imgSpace[0]);
			}
		}
}

void MineArray::m_findMore(int x, int y)
{
	if(x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	BlockType tp = m_block[x][y]->getType();
	if(m_block[x][y]->getNum() == 0 && tp == NORMAL)
	{
		m_block[x][y]->setType(FOUND);
		--m_blockNum;
		m_findMore(x-1,y);
		m_findMore(x,y-1);
		m_findMore(x-1, y-1);
		m_findMore(x+1,y);
		m_findMore(x+1, y-1);
		m_findMore(x,y+1);
		m_findMore(x-1, y+1);
		m_findMore(x+1, y+1);
	}
	else if(m_block[x][y]->getNum() > 0 && tp != FOUND)
	{
		m_block[x][y]->setType(FOUND);
		--m_blockNum;
	}
}

bool MineArray::sweep(int x, int y)
{
	PIXEL2BLOCK(x, y);

	if(m_block[x][y]->isMine() && m_block[x][y]->getType() != FLAG)
	{
		m_firedX = x;
		m_firedY = y;
		return false;
	}
	
	switch(m_block[x][y]->getType())
	{
	case NORMAL:
	case CHOSEN:
	case UNKNOWN:
	case UNKNOWN_CHOSEN:
		m_findMore(x, y);
	default:;
	}

	return true;
}

void MineArray::keyDown(int x, int y)
{
// 	static int lastx, lasty;
// 	if(m_keyCtrl)
// 	{
// 		if(m_block[x][y]->getType() == CHOSEN && lastx != x && lasty != y)
// 		{
// 			m_block[x][y]->setType(SPACE);
// 			m_block[lastx][lasty]->setType(NORMAL);
// 		}
// 	}
// 	else
// 	{
// 		lastx = x;
// 		lasty = y;
// 		if(m_block[x][y]->getType() == CHOSEN)
// 			m_block[x][y]->setType(SPACE);
// 	}
// 	m_keyCtrl = KEYDOWN;
}

void MineArray::guess(int x, int y)
{
	PIXEL2BLOCK(x,y);
	switch(m_block[x][y]->getType())
	{
	case NORMAL:
	case CHOSEN:
		m_block[x][y]->setType(FLAG);
		break;
	case FLAG:
	case FLAG_CHOSEN:
		m_block[x][y]->setType(UNKNOWN);
		break;
	case UNKNOWN:
	case UNKNOWN_CHOSEN:
		m_block[x][y]->setType(NORMAL);
		break;
	default:;
	}
}

void MineArray::moveFrame(int x, int y)
{
	PIXEL2BLOCK(x, y);
	int a = x * IMG_SIZE, b = y * IMG_SIZE;
	switch(m_block[x][y]->getType())
	{
	case NORMAL:
		putimage(a, b, m_imgBlock[1]); break;
	case FLAG:
		putimage(a, b, m_imgFlag[1]); break;
	case UNKNOWN:
		putimage(a, b, m_imgUnknown[1]); break;
	default:;
	}

}

bool MineArray::search(int x, int y)
{
	PIXEL2BLOCK(x,y);
	if(m_block[x][y]->getType() != FOUND)
		return true;
	
	int cnt = 0, tmp = 0, arrX[9], arrY[9];
	bool bombExist = false;
	for(int i = x - 1; i <= x + 1; ++i)
		for(int j = y- 1; j <= y + 1; ++j)
		{
			if(i >= 0 && i < m_width && j >= 0 && j < m_height)
			{
				if(m_block[i][j]->getType() == FLAG)
				{
					++cnt;
				}
				else if(m_block[i][j]->getType() == NORMAL)
				{
					arrX[tmp] = i * IMG_SIZE;
					arrY[tmp] = j * IMG_SIZE;
					++tmp;
				}
				else if(m_block[i][j]->getType() == UNKNOWN)
					return true;
			}
		}
	if(cnt == m_block[x][y]->getNum())
	{
		for(int n = 0; n != tmp; ++n)
		{
			if(!sweep(arrX[n], arrY[n]))
			{
				bombExist = true;
			}
		}
	}

	return !bombExist;
}

void MineArray::winFrame()
{
	float x = g_scrWidth / 2, y = g_scrHeight / 2;	
	cleardevice();
	while(kbhit()) getch();
	for(float dx = RANDF, dy = RANDF;; x += dx, y += dy)
	{
		if(x < 0 || x > g_scrWidth)
			dx = -dx;
		if(y < 0 || y > g_scrHeight)
			dy = -dy;
		cleardevice();
		setcolor(BLACK);
		setfont(35, 35, "宋体");
		render();

		setfont(40,20,"宋体");
		setcolor(RED);
		outtextxy(x, y-50, "You Win!");
		outtextxy(x-100, y+50, "Press anykey to continue!");

		Sleep(10);
		if(kbhit())
		{
			Sleep(100);
			while(kbhit()) getch();
			break;
		}
	}		
}


void MineArray::m_markAll()
{
	for(int i=0; i != m_width; ++i)
		for(int j=0; j != m_height; ++j)
		{
			switch(m_block[i][j]->getType())
			{
			case NORMAL:
				if(m_block[i][j]->isMine())
					m_block[i][j]->setType(MARKEDMINE);
				break;
			case FLAG:
				if(m_block[i][j]->isMine()) m_block[i][j]->setType(FLAG_MINE);
				else m_block[i][j]->setType(WRONGFLAG);
				break;
			case UNKNOWN:
				if(m_block[i][j]->isMine()) m_block[i][j]->setType(UNKNOWN_MINE);
//				else m_block[i][j]->setType(WRONGQMARK);
				break;
			default:;
			}
		}
}

void MineArray::loseFrame()
{

	using std::vector;
	vector<Block*> vec;
	vec.push_back(m_block[m_firedX][m_firedY]);
	m_markAll();
	m_block[m_firedX][m_firedY]->setType(ONFIRE);
	m_block[m_firedX][m_firedY]->setFire();
	while(kbhit()) getch();
 	for(int x, y; ; delay_fps(60))
 	{
		cleardevice();
		setcolor(BLACK);
		setfont(35, 35, "宋体");
		render();
		for(size_t i = 0; i != vec.size(); ++i)
		{
			if(vec[i]->getType() == MINE)
			{
				vec[i]->getPos(x, y);
				putimage(x, y, Mine::imgMine[7]);
				PIXEL2BLOCK(x,y);
				if(m_block[x][y]->isMine() && m_block[x][y]->getType() == MARKEDMINE)
				{
					m_block[x][y]->setType(ONFIRE);
					vec.push_back(m_block[x][y]);
					g_bgm.playBomb();
				}
			}
		}	

		setfont(40,20,"宋体");
		setcolor(YELLOW);
		outtextxy(200,200, "You Lose!");
		outtextxy(50,400, "Press anykey to continue!");

		if(kbhit())
		{
			Sleep(100);
			while(kbhit()) getch();
			break;
		}
 	}
}