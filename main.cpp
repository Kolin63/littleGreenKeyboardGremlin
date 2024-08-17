#include <iostream>
#include <conio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <cstdlib>
#include <time.h>
#include <fstream>

#include "Random.h"

void moveToCoordinate(short x, short y);
void setConsoleColor(int color);
char getInput();
void printKeyboard();
void spawnGremlin();
void checkIfValidWord();
std::wstring stringToWString(const std::string& str);

constexpr short keyboardYStart{ 5 };
constexpr short keyboardXStart{ 0 };
constexpr short keyboardWidth{ 6 };
constexpr short keyboardHeight{ 3 };

constexpr int gremlinTimerMax{ 1500 };
bool runGremlinTimer{ true };
constexpr int pressedKeyTimerMax{ 175 };

constexpr bool showDebugInfo{ true };

int gremlinKey{ -1 };
int pressedKey{ -1 };

std::wstring word{ L"     " };
short lettersGot{ 0 };

bool gameRunning{ true };

int main() {
	// sets to utf8
	_setmode(_fileno(stdout), _O_U8TEXT);

	int deltaTime{};
	int gremlinTimer{ gremlinTimerMax };
	int pressedKeyTimer{ pressedKeyTimerMax };
	while (gameRunning)
	{
		char input{};
		char debugInput{};
		int lastTick{ static_cast<int>(clock()) };

		// GET INPUT
		if (_kbhit())
		{
			input = getInput();
			debugInput = input;
			pressedKeyTimer = pressedKeyTimerMax;
			if (input == '1')
			{
				gremlinTimer = 0;
			}
		}
		else
		{
			input = '0';
		}

		// IF HITTING GREMLIN
		if (gremlinKey == pressedKey && gremlinKey != -1)
		{
			gremlinKey = -1;
			gremlinTimer = 100;
		}

		// TIMER STUFF
		if (gremlinTimer <= 0) 
		{
			gremlinTimer = gremlinTimerMax;
			spawnGremlin();
		}
		if (pressedKeyTimer <= 0)
		{
			pressedKey = -1;
		}
		
		printKeyboard();


		deltaTime = clock() - lastTick;
		if (runGremlinTimer)
		{
			gremlinTimer -= deltaTime;
		}
		pressedKeyTimer -= deltaTime;

		// DEBUG INFO
		if (showDebugInfo)
		{
			moveToCoordinate(0, (keyboardHeight + 1) * 3 + 6);
			std::wcout << "DEBUG\ninput: " << input << "\ndebugInput: " << debugInput << "\ndeltaTime: " << deltaTime
			<< "\ngremlinTimer: " << gremlinTimer << "\nlettersGot: " << lettersGot;
		}

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
	input = static_cast<char>(_getch()); // non blocking
	if (input == '0' || input == '1')
	{
		input = '2';
	}

	char qwerty[] = 
	{
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',       
		'z', 'x', 'c', 'v', 'b', 'n', 'm'                   
	};

	int qwertyValues[] = 
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,   
		10, 11, 12, 13, 14, 15, 16, 17, 18,   
		19, 20, 21, 22, 23, 24, 25   
	};

	for (int i{ 0 }; i <= 25; ++i) {
		if (input == qwerty[i])
		{
			pressedKey = qwertyValues[i];
			break;
		}
	}

	if (input == ' ')
	{
		input = '1';
	}
	else if (gremlinKey == pressedKey && gremlinKey != -1)
	{
		word[lettersGot] = input;
		++lettersGot;
		if (lettersGot == 5)
		{
			runGremlinTimer = false;
			gremlinKey = -1;
			printKeyboard();
			checkIfValidWord();
		}
	}

	return input;
}

void printKeyboard()
{
	moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 3);
	for (int i{ 0 }; i <= word.length(); ++i)
	{
		if (word[i] == 32)
		{
			std::wcout << L"_ ";
		}
		else
		{
			std::wcout << word[i] << ' ';
		}
	}

	for (short i{ 0 }; i <= 25; ++i)
	{
		const wchar_t qwerty[] = 
		{
			'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
			'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
			'Z', 'X', 'C', 'V', 'B', 'N', 'M'
		};
		wchar_t ch{ qwerty[i] };

		short keyboardXOffset{};
		short keyboardYOffset{};

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

		if (i == pressedKey)
		{
			setConsoleColor(10); // green
		}
		else if (i == gremlinKey)
		{
			setConsoleColor(12); // red
		}
		else
		{
			setConsoleColor(8); // gray
		}


		moveToCoordinate(keyboardXOffset, keyboardYOffset);
		std::wcout << L"┌───┐ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 1);
		std::wcout << L"│ " << ch << L" │ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 2);
		std::wcout << L"└───┘ ";

		setConsoleColor(8); // gray

		std::wcout.flush();
	}
}

void setConsoleColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}

void spawnGremlin()
{
	gremlinKey = Random::get(0, 25);
}

void checkIfValidWord()
{
	std::fstream file;
	file.open("validwords.txt", std::ios::in);
	for (int i{ 0 }; i <= 12946; ++i)
	{
		std::string wordChecking;
		std::getline(file, wordChecking);
		std::wstring wwordChecking{ stringToWString(wordChecking) };
		moveToCoordinate(0, (keyboardHeight + 1) * 3 + 3);
		std::wcout << "wordChecking: " << wwordChecking;
		if (wwordChecking == word)
		{
			moveToCoordinate(0, (keyboardHeight + 1) * 3 + 4);
			setConsoleColor(10);
			std::wcout << L"You Win!";
			gameRunning = false;
			return;
		}
	}
	moveToCoordinate(0, (keyboardHeight + 1) * 3 + 4);
	setConsoleColor(12);
	std::wcout << L"Invalid Word. You Lose.";
	gameRunning = false;
	return;
}

std::wstring stringToWString(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}