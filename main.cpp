#include <iostream>
#include <conio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <cstdlib>

void moveToCoordinate(short x, short y);
void setConsoleColor(int color);
char getInput();
void printKeyboard();
void setConsoleBufferSize(int width, int height);
void resizeConsoleWindow(int width, int height);

constexpr short keyboardYStart{ 3 };
constexpr short keyboardXStart{ 0 };

int main() {
	// sets to utf8
	_setmode(_fileno(stdout), _O_U8TEXT);

	while (true)
	{
		[[maybe_unused]]
		char input{ getInput() };
		printKeyboard();

		std::cout.flush();
	}
	return 0;
}


void moveToCoordinate(short x, short y)
{
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

char getInput()
{
	char input{};
	if (_kbhit()) // if keyboard key is hit
	{
		input = static_cast<char>(_getch()); // non blocking
	}
	else
	{
		input = 48; // 0 in ascii
	}
	return input;
}

void printKeyboard()
{
	std::wcout.clear();
	for (short i{ 0 }; i <= 25; ++i)
	{
		wchar_t qwerty[] = 
		{
			'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
			'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
			'Z', 'X', 'C', 'V', 'B', 'N', 'M'
		};
		wchar_t ch{ qwerty[i] };

		short keyboardXOffset{};
		short keyboardYOffset{};

		short keyboardWidth{ 6 };
		short keyboardHeight{ 3 };

		if (i <= 9)
		{
			keyboardXOffset = ((i - 0) * keyboardWidth) + 0 + keyboardXStart;
			keyboardYOffset = (keyboardHeight * 0) + keyboardYStart;
		}
		else if (i <= 18)
		{
			keyboardXOffset = ((i - 10) * keyboardWidth) + static_cast<short>(keyboardWidth * 1 / 3) + keyboardXStart;
			keyboardYOffset = (keyboardHeight * 1) + keyboardYStart;
		}
		else if (i <= 25)
		{
			keyboardXOffset = ((i - 19) * keyboardWidth) + static_cast<short>(keyboardWidth * 2 / 3) + keyboardXStart;
			keyboardYOffset = (keyboardHeight * 2) + keyboardYStart;
		}

		moveToCoordinate(keyboardXOffset, keyboardYOffset);
		std::wcout << L"┌───┐ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 1);
		std::wcout << L"│ " << ch << L" │ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 2);
		std::wcout << L"└───┘ ";

		std::wcout.flush();
	}
}

void setConsoleColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}