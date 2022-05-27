#include <iostream>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <string>
#include <wtypes.h>
#include <random>
#include <time.h>
#include <chrono>
#include <map>

using namespace std;

// Handling params
std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
int iter = 0;
int width = 100;
int height = 50;
std::uniform_int_distribution<int> uni(0, width); // guaranteed unbiased
std::uniform_int_distribution<int> unis(0, height);
std::uniform_int_distribution<int> dev(-1, 2);

class Snowflake {
public:
	bool isDead = false;
	int x = 0;
	int y = 0;
	Snowflake(int x, int y) {
		this->x = x;
		this->y = y;
	}
	void fall() {
		this->y++;
		this->x += dev(rng);
	}
};

// Screen and game
wchar_t* screen = new wchar_t[width * height];
vector<Snowflake> snow;

void sClean() {
	for (int i = 0; i < width * height; i++) {
		screen[i] = ' ';
	}
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void SetWindow(int Width, int Height)
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;
	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Handle, coord);
	SetConsoleWindowInfo(Handle, TRUE, &Rect);
}
void sSet(char symbol, int row, int column) { screen[((column)*width) + row] = symbol; } // Метод для поcтановки символа в нужную часть матрицы
void sSet(string s, int column, int row)
{
	for (int i = 0; i < s.size(); i++)
	{
		screen[((column)*width) + row + i] = s[i];
	}
}

void handleSnow() {
	snow.push_back(Snowflake(uni(rng), unis(rng)));
}
void drawCatch(int x, int y) {
	/*sSet('|', x, y);
	for (int i = 1; i < 10; i++) {
		sSet('_', x + i, y);
	}
	sSet('|', x + 10, y);*/
	sSet('(', x, y);
	sSet('=', x + 1, y);
	sSet(')', x+2, y);
}
int score = 0;
int main()
{
	auto startTime = std::chrono::system_clock::now();
	/*auto random_integer = uni(rng);*/
	POINT p;
	// init
	SetWindow(width, height);
	DWORD dwBytesWritten = 0;
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	sClean();
	int screenWidth, screenHeight;
	GetDesktopResolution(screenWidth, screenHeight);
	double ax, ay, px, py;
	// handle
	for (;;) {
		sClean();
		if (GetCursorPos(&p))
		{
			px = p.x;
			py = p.y;
			string tpX = to_string(p.x);
			string tpY = to_string(p.y);
		}
		ax = round(width * px / screenWidth);
		ay = round(height * py / screenHeight);
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() / 1000 >= iter) {
			iter++;
			handleSnow();
			for (int i = 0; i < snow.size(); i++) {
				snow[i].fall();
				if (snow[i].x - ax <= 2 && snow[i].y - ay <= 2 && snow[i].isDead == false) {
					//snow.erase(snow.begin() + i);
					snow[i].isDead = true;
					score++;
				}
				snow[i].y++;
			}
		}
		for (auto i : snow) {
			//i.fall();
			if (i.isDead) { continue; }
			sSet('*', i.x, i.y);
		}
		sSet(to_string(score), 0, 0);
		drawCatch(ax, ay);
		WriteConsoleOutputCharacter(hConsole, screen, width * height, { 0, 0 }, &dwBytesWritten);
	}
}