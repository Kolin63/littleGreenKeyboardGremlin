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
void printMenu();
wchar_t intToQwertyChar(int x, bool capital);
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
std::string gameState{ "menu" };

bool zoomedIn{ false };
std::wstring attackWord{ L"     " };
wchar_t attackWordLettersGot{ 0 };
wchar_t attackWordFirstLetter{};
int gremlinMaxHealth{ 100 };
int gremlinHealth{ gremlinMaxHealth };
int attackWordsGot{ 0 };

int baitedKey{ -1 };
int bait{};

int main() {
	// sets to utf8
	[[maybe_unused]]
	int sillyReturnValue = _setmode(_fileno(stdout), _O_U8TEXT);

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
			if (gameState == "game")
			{
				pressedKeyTimer = pressedKeyTimerMax;
				if (input == '1' && gremlinKey != -1)
				{
					runGremlinTimer = false;
					attackWordFirstLetter = intToQwertyChar(gremlinKey, false);
					gremlinKey = -1;
					attackWordLettersGot = 1;
					attackWord = L"     ";
					attackWord[0] = attackWordFirstLetter;
					gremlinHealth = gremlinMaxHealth;
					attackWordsGot = 0;

					zoomedIn = true;
				}
			}
			if (gameState == "menu" && input == '1')
			{
				moveToCoordinate(0, 0);
				std::wcout << L"                                                \n                           ";
				gameState = "game";
			}
		}
		else
		{
			input = '0';
		}

		if (gameState == "game")
		{
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
		}

		if (gameState == "menu")
		{
			printMenu();
		}

		deltaTime = clock() - lastTick;
		if (runGremlinTimer && gameState == "game")
		{
			gremlinTimer -= deltaTime;
		}
		if (gameState == "game")
		{
			pressedKeyTimer -= deltaTime;
		}

		// DEBUG INFO
		if (showDebugInfo)
		{
			moveToCoordinate(80, 0);
			std::wcout << "DEBUG";
			moveToCoordinate(80, 1);
			std::wcout << "input: " << input;
			moveToCoordinate(80, 2);
			std::wcout << "debugInput : " << debugInput;
			moveToCoordinate(80, 3);
			std::wcout << "deltaTime : " << deltaTime;
			moveToCoordinate(80, 4);
			std::wcout << "gremlinTimer: " << gremlinTimer;
			moveToCoordinate(80, 5);
			std::wcout << "lettersGot: " << lettersGot;
			moveToCoordinate(80, 6);
			std::wcout << "attackWordLettersGot: " << attackWordLettersGot;
			moveToCoordinate(80, 7);
			std::wcout << "attackWordFirstLetter: " << attackWordFirstLetter;
			moveToCoordinate(80, 8);
			std::wcout << "gremlinKey: " << gremlinKey;
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
	char qwertyCapital[] =
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M'
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
		if (input == qwertyCapital[i])
		{
			baitedKey = qwertyValues[i];
		}
	}

	if (input == ' ')
	{
		input = '1';
	}
	else if (gremlinKey == pressedKey && gremlinKey != -1 && !zoomedIn)
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
	else if (zoomedIn)
	{
		attackWord[attackWordLettersGot] = input;
		++attackWordLettersGot;
		if (attackWordLettersGot == 5)
		{
			++attackWordsGot;
			attackWord = L"     ";
			attackWord[0] = attackWordFirstLetter;
			attackWordLettersGot = 1;
		}
	}

	return input;
}

void printKeyboard()
{
	moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 3);
	std::wstring wordToPrint{ L"     " };
	wordToPrint = !zoomedIn ? word : attackWord;
	for (int i{ 0 }; i <= wordToPrint.length(); ++i)
	{
		if (wordToPrint[i] == 32)
		{
			std::wcout << L"_ ";
		}
		else
		{
			std::wcout << wordToPrint[i] << ' ';
		}
	}

	const wchar_t qwerty[] =
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M'
	};

	if (zoomedIn)
	{
		moveToCoordinate(keyboardXStart, keyboardYStart);
		std::wcout << L"           ┌────────────────────────────────────┐           \n";
		for (int i{ 0 }; i <= 17; ++i)
		{
			std::wcout << L"           │                                    │           \n";
		}
		std::wcout << L"           └────────────────────────────────────┘           \n";

		return;
	}

	for (short i{ 0 }; i <= 25; ++i)
	{
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
		else if (i == baitedKey)
		{
			setConsoleColor(14); // yellow
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
	if (baitedKey != -1)
	{
		gremlinKey = baitedKey;
		baitedKey = -1;
		return;
	}
	gremlinKey = Random::get(0, 25);
}

void checkIfValidWord()
{
	std::fstream file;
	file.open("validwords.txt", std::ios::in);

	int startLines[] =
	{
		1, 737, 1645, 2565, 3246, 3549, 4144, 4781, 5269,
		5434, 5636, 6011, 6586, 7279, 7604, 7866, 8723, 8801,
		9429, 10988, 11804, 11993, 12235, 12646, 12662, 12843, 12946
	};
	wchar_t firstChar{ word[0] };
	int startLine{ startLines[static_cast<int>(firstChar) - 97] - 1 };
	int endLine{ startLines[static_cast<int>(firstChar) - 96] - 1};
	

	for (int i{ 0 }; i <= endLine; ++i)
	{
		std::string wordChecking;
		std::getline(file, wordChecking);

		if (i < startLine) continue;

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

void printMenu()
{
	moveToCoordinate(0, 0);
	std::wcout << L"Welcome to Little Green Keyboard Gremlin!\nPress [ Space ] to start";
}

wchar_t intToQwertyChar(int x, bool capital)
{
	wchar_t qwerty[] =
	{
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
		'z', 'x', 'c', 'v', 'b', 'n', 'm'
	};
	wchar_t qwertyCapital[] =
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M'
	};

	if (!capital)
	{
		return qwerty[x];
	}
	return qwertyCapital[x];
}