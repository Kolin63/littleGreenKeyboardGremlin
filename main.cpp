#include <iostream>
#include <conio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <thread>

#include "Random.h"

void moveToCoordinate(int x, int y);
void setConsoleColor(int color);
void setConsoleColor(int color, int backgroundColor);
char getInput();
void printKeyboard();
void spawnGremlin();
bool checkIfValidWord(std::wstring theWord);
void printMenu();
wchar_t intToQwertyChar(int x, bool capital);
void printScoreboard();
void clearScreen();
void waitMilliseconds(int x);
std::wstring stringToWString(const std::string& str);

constexpr short keyboardYStart{ 5 };
constexpr short keyboardXStart{ 0 };
constexpr short keyboardWidth{ 6 };
constexpr short keyboardHeight{ 3 };

constexpr int consoleBlack{ 0 };
constexpr int consoleBlue{ 1 };
constexpr int consoleGreen{ 2 };
constexpr int consoleAqua{ 3 };
constexpr int consoleRed{ 4 };
constexpr int consolePurple{ 5 };
constexpr int consoleYellow{ 6 };
constexpr int consoleWhite{ 7 };
constexpr int consoleGrey{ 8 };
constexpr int consoleLightBlue{ 9 };
constexpr int consoleLightGreen{ 10 };
constexpr int consoleLightAqua{ 11 };
constexpr int consoleLightRed{ 12 };
constexpr int consoleLightPurple{ 13 };
constexpr int consoleLightYellow{ 14 };
constexpr int consoleBrightWhite{ 15 };

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
int attackWordsGot{ 0 };
std::wstring attackWords[5] { L"     ", L"     ", L"     " , L"     " , L"     " };

int gremlinMaxHealth{ 100 };
int gremlinHealth{ gremlinMaxHealth };

int baitedKey{ -1 };
int bait{ 2 };

int keyboardHealth{ 100 };
int gameRound{ 0 };
int coins{ 0 };
int score{ 0 };
int letterValues[26] =
{
	20, 18,  8, 14, 11, 18, 18, 13, 13, 19,
	12, 14, 16, 18, 18, 14, 20, 20, 16,
	20, 20, 18, 20, 19, 14, 18
};

int main() {
	// sets to utf8
	[[maybe_unused]]
	int sillyReturnValue = _setmode(_fileno(stdout), _O_U8TEXT);
	setConsoleColor(consoleBrightWhite, consoleBlack);

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

					for (int i{ 0 }; i <= 4; ++i)
					{
						attackWords[i] = L"     ";
					}

					zoomedIn = true;
				}
			}
			if (gameState == "menu" && input == '1')
			{
				moveToCoordinate(0, 0);
				clearScreen();
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
			printScoreboard();
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
			moveToCoordinate(80, 15);
			std::wcout << "DEBUG";
			moveToCoordinate(80, 16);
			std::wcout << "input: " << input;
			moveToCoordinate(80, 17);
			std::wcout << "debugInput : " << debugInput;
			moveToCoordinate(80, 18);
			std::wcout << "deltaTime : " << deltaTime;
			moveToCoordinate(80, 19);
			std::wcout << "gremlinTimer: " << gremlinTimer;
			moveToCoordinate(80, 20);
			std::wcout << "lettersGot: " << lettersGot;
			moveToCoordinate(80, 21);
			std::wcout << "attackWordLettersGot: " << attackWordLettersGot;
			moveToCoordinate(80, 22);
			std::wcout << "attackWordFirstLetter: " << attackWordFirstLetter;
			moveToCoordinate(80, 23);
			std::wcout << "gremlinKey: " << gremlinKey;
			moveToCoordinate(80, 24);
			std::wcout << "attackWord: " << attackWord;
		}

		std::cout.flush();
	}
	return 0;
}


void moveToCoordinate(int x, int y)
{
	COORD coord = { static_cast<short>(x), static_cast<short>(y) };
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
		if (input == qwertyCapital[i] && bait > 0)
		{
			--bait;
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
		
		coins += letterValues[pressedKey];
		score += letterValues[pressedKey];

		if (lettersGot == 5)
		{
			runGremlinTimer = false;
			gremlinKey = -1;
			printKeyboard();
			checkIfValidWord(word);
			gameRunning = false;
		}
	}
	else if (zoomedIn && input != '1' && input != '0' && input != '\b')
	{
		attackWord[attackWordLettersGot] = input;
		++attackWordLettersGot;

		coins += static_cast<int>(letterValues[pressedKey] / 5);
		score += static_cast<int>(letterValues[pressedKey] / 5);

		if (attackWordLettersGot == 5)
		{
			printKeyboard();
			bool isRepeatWord{ false };
			for (int i{ 0 }; i <= 4; ++i)
			{
				if (attackWords[i] == attackWord)
				{
					isRepeatWord = true;
				}
			}
			if (checkIfValidWord(attackWord) && !isRepeatWord)
			{
				attackWords[attackWordsGot] = attackWord;
				++attackWordsGot;
				attackWord = L"     ";
				attackWord[0] = attackWordFirstLetter;
				attackWordLettersGot = 1;
				setConsoleColor(consoleLightGreen);
			}
			else
			{
				keyboardHealth -= 10;
				zoomedIn = false;
				runGremlinTimer = true;
				setConsoleColor(consoleLightRed);
				moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 1);
				for (int i{ 0 }; i <= attackWord.length(); ++i)
				{
					std::wcout << attackWord[i] << ' ';
				}
				waitMilliseconds(1000);
				clearScreen();
			}
		}
	}

	return input;
}

void printKeyboard()
{
	if (zoomedIn) setConsoleColor(consoleGrey);
	else setConsoleColor(consoleBrightWhite);
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
	if (zoomedIn)
	{
		setConsoleColor(consoleBrightWhite);
		moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 1);
		for (int i{ 0 }; i <= attackWord.length(); ++i)
		{
			if (attackWord[i] == 32)
			{
				std::wcout << L"_ ";
			}
			else
			{
				std::wcout << attackWord[i] << ' ';
			}
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
		setConsoleColor(consoleBrightWhite);

		moveToCoordinate(keyboardXStart + 11, keyboardYStart);
		std::wcout << L"┌────────────────────────────────────┐";
		for (int i{ 0 }; i <= 17; ++i)
		{
			moveToCoordinate(keyboardXStart + 11, keyboardYStart + i + 1);
			std::wcout << L"│                                    │";
		}
		moveToCoordinate(keyboardXStart + 11, keyboardYStart + 19);
		std::wcout << L"└────────────────────────────────────┘";

		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 0);
		std::wcout << "ATTACK WORDS";
		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 1);
		std::wcout << '-' << attackWords[0];
		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 2);
		std::wcout << '-' << attackWords[1];
		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 3);
		std::wcout << '-' << attackWords[2];
		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 4);
		std::wcout << '-' << attackWords[3];
		moveToCoordinate(keyboardXStart + 50, keyboardYStart + 5);
		std::wcout << '-' << attackWords[4];
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
			setConsoleColor(consoleLightGreen);
		}
		else if (i == baitedKey)
		{
			setConsoleColor(consoleLightYellow); 
		}
		else if (i == gremlinKey)
		{
			setConsoleColor(consoleLightRed);
		}
		else
		{
			setConsoleColor(consoleGrey); 
		}


		moveToCoordinate(keyboardXOffset, keyboardYOffset);
		std::wcout << L"┌───┐ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 1);
		std::wcout << L"│ " << ch << L" │ ";

		moveToCoordinate(keyboardXOffset, keyboardYOffset + 2);
		std::wcout << L"└───┘ ";

		setConsoleColor(consoleGrey); 

		std::wcout.flush();
	}
}

void setConsoleColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}

void setConsoleColor(int color, int backgroundColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (static_cast<WORD>(backgroundColor << 4)) | static_cast<WORD>(color));
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

bool checkIfValidWord(std::wstring theWord)
{
	std::fstream file;
	file.open("validwords.txt", std::ios::in);

	int startLines[] =
	{
		1, 737, 1645, 2565, 3246, 3549, 4144, 4781, 5269,
		5434, 5636, 6011, 6586, 7279, 7604, 7866, 8723, 8801,
		9429, 10988, 11804, 11993, 12235, 12646, 12662, 12843, 12946
	};
	wchar_t firstChar{ theWord[0] };
	int startLine{ startLines[static_cast<int>(firstChar) - 97] - 1 };
	int endLine{ startLines[static_cast<int>(firstChar) - 96] - 1};
	

	for (int i{ 0 }; i <= endLine; ++i)
	{
		std::string wordChecking;
		std::getline(file, wordChecking);

		if (i < startLine) continue;

		std::wstring wwordChecking{ stringToWString(wordChecking) };

		if (wwordChecking == theWord)
		{
			moveToCoordinate(0, (keyboardHeight + 1) * 3 + 4);
			return true;
		}
	}
	moveToCoordinate(0, (keyboardHeight + 1) * 3 + 4);
	return false;
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

void printScoreboard()
{
	setConsoleColor(consoleBrightWhite);
	moveToCoordinate(70, 2);
	std::wcout << "LITTLE GREEN KEYBOARD GREMLIN";
	moveToCoordinate(70, 3);
	std::wcout << "Keyboard Health.." << keyboardHealth << "   ";
	moveToCoordinate(70, 4);
	std::wcout << "Bait............." << bait << "   ";
	moveToCoordinate(70, 5);
	std::wcout << "Coins............" << coins << "   ";
	moveToCoordinate(70, 6);
	std::wcout << "Score............" << score << "   ";
	moveToCoordinate(70, 7);
	std::wcout << "Round............" << gameRound << "   ";
}

void clearScreen()
{
	for (int i{ 0 }; i <= 24; ++i)
	{
		for (int j{ 0 }; j <= 79; ++j)
		{
			moveToCoordinate(j, i);
			std::wcout << ' ';
		}
	}
}

void waitMilliseconds(int x)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(x));
}
