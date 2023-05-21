#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <graphics.h>
#include <thread>
#include <cstring>
#include <Commdlg.h>

#ifdef _DEBUG
bool ShowConsole = true;
#else
bool ShowConsole = false;
#endif

#define WALL				0	// 墙
#define BRICK_R				1	// 红
#define BRICK_Y				2	// 黄
#define BRICK_G				3	// 绿
#define BRICK_C				4	// 青
#define BRICK_B				5	// 蓝
#define BRICK_W				6	// 白

// 方块颜色
COLORREF Colors[10] = {
	RGB(163, 163, 162),	// 墙：灰色
	RGB(214, 63, 37),	// 红 R
	RGB(228, 174, 31),	// 黄 Y
	RGB(15, 216, 15),	// 绿 G
	RGB(39, 253, 251),	// 青 C
	RGB(68, 112, 202),	// 蓝 B
	RGB(234, 234, 234),	// 白 W
	WHITE,
};

//按钮位置
RECT CC[10] = {
	{10, 10, 30, 30},	// 墙：灰色
	{10, 40, 30, 60},	// 红 R
	{10, 70, 30, 90},	// 黄 Y
	{10, 100, 30, 120},	// 绿 G
	{10, 130, 30, 150},	// 青 C
	{10, 160, 30, 180},	// 蓝 B
	{10, 190, 30, 210},	// 白 W
	{10, 220, 30, 240}, // 橡皮才
	{10, 310, 30, 330}, // 清空
	{10, 340, 30, 360}, // 保存
};

RECT CLEAR = { 10, 310, 30, 330 }; // 清空
RECT SAVE = { 10, 340, 30, 370 }; // 清空

int chooser = WALL;
ExMessage msg;
const char c[10] = { '#', 'R', 'Y', 'G', 'C', 'B', 'W', ' '};

struct LOGMAP {
	char map[100][100];
	int width = 31, height = 51;
};

LOGMAP Map;

FILE* fp; /*文件指针*/


char* LPWSTR2LPSTR(LPWSTR lpwszStrIn);

char* chooseFiles();

bool IsMouseIn(RECT loc);

bool IsMouseIn(int left, int top, int right, int bottom);

void drawChoosers();

void drawFrame(int x, int y);

void drawBlocks(int x, int y);

void Graph();


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	if (ShowConsole)
		initgraph(720, 480, SHOWCONSOLE);
	else
		initgraph(720, 480);

	std::thread graphics(Graph);
	graphics.detach();
	
	memset(Map.map, ' ', sizeof(Map.map));

	BeginBatchDraw();
	while (true)
	{
		while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
			flushmessage();
			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				std::cout << "[DEBUG]" << "mouse Lbutton donw[" << "x: " << msg.x << "|y: " << msg.y << "]" << std::endl;
				//处理颜色和橡皮擦按键
				for (int i = 0; i < 8; i++)
				{
					if (IsMouseIn(CC[i]))
					{
						chooser = i;
						std::cout << "[DEBUG]" << "chooser have changed to" << i << std::endl;
					}
				}
				//清空
				if (IsMouseIn(CLEAR))
				{
					if (MessageBox(GetHWnd(), L"确定要清空吗", L"!", MB_OKCANCEL) != 1)
						break;
					for (int i = 0; i < 51; i++)
						for (int j = 0; j < 31; j++)
							Map.map[i][j] = ' ';
				}
				//保存
				if (IsMouseIn(SAVE))
				{ 
					char* path = chooseFiles();
					if ((fp = fopen(path, "a")) == NULL)
					{
						std::cout << "Error" << std::endl;
					}

					for (int i = 0; i < 31; i++)
					{
						for (int j = 0; j < 51; j++)
						{
							fputc(Map.map[i][j], fp);
						}
						fputc('\n', fp);
					}
					fclose(fp);
					std::cout << "[DEBUG]" << "Save to " << path << std::endl;
				}
				break;
			}
			if (IsMouseIn(50, 20, 50 + 51 * 12, 30 + 31 * 12) && msg.lbutton)
				Map.map[(msg.x - 50) / 12][(msg.y - 20) / 12] = c[chooser];
			Sleep(14);
		}
	}
	std::cin.get();
	EndBatchDraw();
}

char* LPWSTR2LPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	if (lpwszStrIn != NULL) {
		int nInputStrLen = wcslen(lpwszStrIn);
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
		pszOut = new char[nOutputStrLen];
		if (pszOut != NULL) {
			memset(pszOut, 0x00, nOutputStrLen);
			WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
		}
	}
	return  pszOut;
}

char* chooseFiles()
{
	OPENFILENAME ofn;
	TCHAR szOpenFileNames[80 * MAX_PATH] = { 0 };
	TCHAR szPath[MAX_PATH];
	TCHAR szFileName[80 * MAX_PATH];

	int nLen = 0;
	TCHAR* p = NULL;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHWnd();
	ofn.lpstrFile = szOpenFileNames;
	ofn.nMaxFile = sizeof(szOpenFileNames);
	ofn.lpstrFile[0] = _T('\0');
	ofn.lpstrFilter = _T(".dat\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = _T("请选择");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_EXPLORER;
	if (!::GetSaveFileName(&ofn)) {
		return new char[0];
	}
	lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
	szPath[ofn.nFileOffset] = '\0';
	nLen = lstrlen(szPath);
	if (szPath[nLen - 1] != '\\') {
		lstrcat(szPath, _T("\\"));
	}
	p = szOpenFileNames + ofn.nFileOffset;
	ZeroMemory(szFileName, sizeof(szFileName));

	std::string str = "";
	while (*p) {
		std::string fileName = LPWSTR2LPSTR(p);
		std::string filePath = LPWSTR2LPSTR(szPath);
		std::string completePath = filePath + fileName;

		str += completePath + ".dat";
		p += lstrlen(p) + 1;
	}

	// 将string转为char*
	char* strc = new char[strlen(str.c_str()) + 1];
	const char* cc = str.c_str();
	strcpy_s(strc, str.size() + 1, cc);

	return strc;
}

bool IsMouseIn(RECT loc)
{
	return msg.x > loc.left && msg.y > loc.top && msg.x < loc.right && msg.y < loc.bottom;
}

bool IsMouseIn(int left, int top, int right, int bottom)
{
	return msg.x > left && msg.y > top && msg.x < right && msg.y < bottom;
}

void drawChoosers()
{
	setlinecolor(RGB(100, 100, 100));
	setlinestyle(0, 2);
	rectangle(CC[chooser].left - 2, CC[chooser].top - 2, CC[chooser].right + 3, CC[chooser].bottom + 3);
	setlinecolor(RGB(150, 150, 150));
	for (int i = 0; i < 10; i++)
		if (IsMouseIn(CC[i]) && chooser != i)
			rectangle(CC[i].left - 2, CC[i].top - 2, CC[i].right + 3, CC[i].bottom + 3);

	for (int i = 0; i < 7; i++)
	{
		setfillcolor(Colors[i]);
		solidrectangle(CC[i].left, CC[i].top, CC[i].right, CC[i].bottom);
	}
	settextstyle(20, 20, L"");
	settextcolor(RGB(120, 120, 120));
	outtextxy(10, 220, L"E");
	outtextxy(10, 310, L"C");
	outtextxy(10, 340, L"S");
}

void drawFrame(int x, int y)
{
	setlinecolor(RGB(200, 200, 200));
	setlinestyle(0, 1);
	for (int i = 0; i <= 51; i++)
		line(x + i * 12, y, x + i * 12, y + 31 * 12);
	for (int i = 0; i <= 31; i++)
		line(x, y + i * 12, x + 51 * 12, y + i * 12);
}

void drawBlocks(int x, int y)
{
	for(int i = 0;i < 51;i++)
		for (int j = 0; j < 31; j++)
		{
			switch (Map.map[i][j])
			{
			case '#':
				setfillcolor(Colors[WALL]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'R':
				setfillcolor(Colors[BRICK_R]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'Y':
				setfillcolor(Colors[BRICK_Y]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'G':
				setfillcolor(Colors[BRICK_G]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'C':
				setfillcolor(Colors[BRICK_C]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'B':
				setfillcolor(Colors[BRICK_B]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case 'W':
				setfillcolor(Colors[BRICK_W]);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			case ' ':
				setfillcolor(WHITE);
				solidrectangle(x + 2 + 12 * i, y + 2 + 12 * j, x + 10 + 12 * i, y + 10 + 12 * j);
				break;
			}

		}
}

void Graph()
{
	while (true)
	{
		setbkcolor(RGB(255, 255, 255));
		cleardevice();
		drawBlocks(50, 20);
		drawChoosers();
		drawFrame(50, 20);
		FlushBatchDraw();
		Sleep(14);
	}
}

