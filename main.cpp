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

void printKeyboard();
void printShop();
void printMenu();
void printScoreboard();
void printCinematic();
void printHelp();
void clearScreen();

void moveToCoordinate(int x, int y);
void setConsoleColor(int color);
void setConsoleColor(int color, int backgroundColor);
char getInput();
void waitMilliseconds(int x);
void cinematicWrite(std::wstring text, int xLeft = 0, int startY = 0);

bool checkIfValidWord(std::wstring theWord);
wchar_t intToQwertyChar(int x, bool capital);
std::wstring stringToWString(const std::string& str);

void spawnGremlin();
void loseState();
void playAgain();

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

constexpr int gremlinTimerMaxRef{ 10000 };
int gremlinTimerMax{ gremlinTimerMaxRef };
bool runGremlinTimer{ true };
bool gremlinTimerDamage{ false };
constexpr int pressedKeyTimerMax{ 175 };

int gremlinTimer{ 1000 };
int attackTimer{ 0 };
int pressedKeyTimer{ 0 };

constexpr bool showDebugInfo{ false };

int gremlinKey{ -1 };
int pressedKey{ -1 };

std::wstring word{ L"     " };
short lettersGot{ 0 };

bool gameRunning{ true };
std::string gameState{ "cinematic" };

bool zoomedIn{ false };
std::wstring attackWord{ L"     " };
wchar_t attackWordLettersGot{ 0 };
wchar_t attackWordFirstLetter{};
bool attackWordFirstLetterProtection{ true };
int attackWordsGot{ 0 };
std::wstring attackWords[3] { L"     ", L"     ", L"     " };

int baitedKey{ -1 };
int maxBait{ 3 };
int bait{ 3 };

int keyboardMaxHealth{ 5 };
int keyboardHealth{ keyboardMaxHealth };
int gameRound{ 0 };
int coins{ 0 };
int score{ 0 };
int letterValues[26] =
{
	20, 18,  8, 14, 11, 18, 18, 13, 13, 19,
	12, 14, 16, 18, 18, 14, 20, 20, 16,
	20, 20, 18, 20, 19, 14, 18
};

int selectedMenuItem{ 0 };
int maxMenuItem{};

constexpr int keyboardMaxHealthPrice{ 40 };
constexpr int healPrice{ 30 };
constexpr int maxBaitPrice{ 100 };
constexpr int baitPrice{ 30 };
constexpr int slowerTimerPrice{ 30 };
constexpr int moreCoinsPrice{ 40 };

int slowerTimerBonus{};
int moreCoinsBonus{};

bool wonPrevious{ false };

int main() 
{
	// sets to utf8
	[[maybe_unused]]
	int sillyReturnValue = _setmode(_fileno(stdout), _O_U8TEXT);
	setConsoleColor(consoleBrightWhite, consoleBlack);

	int deltaTime{};
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
					attackWordLettersGot = 1;
					attackWord = L"     ";
					attackWord[0] = attackWordFirstLetter;
					attackWordsGot = 0;
					attackTimer = gremlinTimerMax * 3;
					attackWordFirstLetterProtection = true; 

					for (int i{ 0 }; i <= 2; ++i)
					{
						attackWords[i] = L"     ";
					}

					zoomedIn = true;
				}

				if (input == '`')
				{
					loseState();
				}

				if constexpr (showDebugInfo && input == '~')
				{
					word = L"crate";
					coins += 10000;
					lettersGot = 5;
				}

				if (lettersGot == 5)
				{
					runGremlinTimer = false;
					gremlinKey = -1;
					printKeyboard();
					if (checkIfValidWord(word))
					{
						gameState = "win";
						moveToCoordinate(keyboardWidth * 5 - 17, keyboardYStart - 2);
						setConsoleColor(consoleLightGreen);
						std::wcout << L"You win! Press space to continue.";
						selectedMenuItem = 0;
					}
					else
					{
						loseState();
					}
				}
			}
			if (gameState == "menu")
			{
				maxMenuItem = 2;
				// hmpk
				if (input == 'H')
				{
					--selectedMenuItem;
					if (selectedMenuItem < 0) selectedMenuItem = 0;
					else clearScreen();
				}
				if (input == 'P')
				{
					++selectedMenuItem;
					if (selectedMenuItem > maxMenuItem) selectedMenuItem = maxMenuItem;
					else clearScreen();
				}

				if (input == '\r')
				{
					switch (selectedMenuItem)
					{
					case 0:
						wonPrevious = false;
						playAgain();
						break;
					case 1:
						clearScreen();
						gameState = "help";
						break;
					case 2:
						gameRunning = false;
					}
				}
			}
			if (gameState == "win" && input == '1')
			{
				clearScreen();
				gameState = "shop";
			}
			if (gameState == "lose" && input == '1')
			{
				clearScreen();
				gameState = "menu";
			}
			if (gameState == "shop")
			{
				maxMenuItem = 6;
				// hmpk
				if (input == 'H')
				{
					--selectedMenuItem;
					if (selectedMenuItem < 0) selectedMenuItem = 0;
				}
				if (input == 'P')
				{
					++selectedMenuItem;
					if (selectedMenuItem > maxMenuItem) selectedMenuItem = maxMenuItem; 
				}
				if (input == '\r')
				{
					switch (selectedMenuItem)
					{
					case 0:
						if (coins < keyboardMaxHealthPrice) break;
						++keyboardMaxHealth;
						coins -= keyboardMaxHealthPrice;
						break;
					case 1:
						if (coins < healPrice || keyboardHealth >= keyboardMaxHealth) break;
						++keyboardHealth;
						coins -= healPrice;
						break;
					case 2:
						if (coins < maxBaitPrice) break;
						++maxBait;
						coins -= maxBaitPrice;
						break;
					case 3:
						if (coins < baitPrice || bait >= maxBait) break;
						++bait;
						coins -= baitPrice;
						break;
					case 4:
						if (coins < slowerTimerPrice) break;
						++slowerTimerBonus;
						coins -= slowerTimerPrice;
						break;
					case 5:
						if (coins < moreCoinsPrice) break;
						++moreCoinsBonus;
						coins -= moreCoinsPrice;
						break;
					case 6:
						wonPrevious = true;
						playAgain();
						break;
					}
					clearScreen();
				}
			}
			if (gameState == "help" && input == '1')
			{
				gameState = "menu";
				clearScreen();
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
				gremlinTimerDamage = false;
			}

			// TIMER STUFF
			if (gremlinTimer <= 0)
			{
				if (gremlinTimerDamage)
				{
					keyboardHealth -= 2;
				}
				else
				{
					gremlinTimerDamage = true;
				}
				gremlinTimer = gremlinTimerMax;
				spawnGremlin();
			}
			if (pressedKeyTimer <= 0)
			{
				pressedKey = -1;
			}
			if (attackTimer <= 0 && zoomedIn)
			{
				keyboardHealth -= 1;
				zoomedIn = false;
				runGremlinTimer = true;
				gremlinTimerDamage = false;
				gremlinKey = -1;
				setConsoleColor(consoleLightRed);
				moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 1);
				for (int i{ 0 }; i <= attackWord.length(); ++i)
				{
					std::wcout << attackWord[i] << ' ';
				}
				waitMilliseconds(1000);
				clearScreen();
				attackWordFirstLetterProtection = true;
			}
			if (keyboardHealth <= 0)
			{
				loseState();
			}

			printKeyboard();
			printScoreboard();
		}

		if (gameState == "menu")
		{
			printMenu();
		}

		if (gameState == "shop")
		{
			printShop();
		}

		if (gameState == "cinematic")
		{
			maxMenuItem = 100;
			if (input == '1' && selectedMenuItem < maxMenuItem)
			{
				++selectedMenuItem;
			}
			if (selectedMenuItem == maxMenuItem)
			{
				clearScreen();
				gameState = "menu";
			}
			if (input == '\r' && selectedMenuItem == 1)
			{
				selectedMenuItem = 4;
			}
		}

		if (gameState == "help")
		{
			printHelp();
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
		if (zoomedIn && gameState == "game")
		{
			attackTimer -= deltaTime;
		}
		if (gameState == "cinematic")
		{
			printCinematic();
		}

		// DEBUG INFO
		if (showDebugInfo)
		{
			setConsoleColor(consoleBrightWhite, consoleBlack);
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
			moveToCoordinate(80, 25);
			std::wcout << "selectedMenuItem: " << selectedMenuItem;
			moveToCoordinate(80, 26);
			std::wcout << "gameState: " << stringToWString(gameState);
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

	for (int i{ 0 }; i <= 25; ++i) 
	{
		if (input == qwerty[i])
		{
			pressedKey = qwertyValues[i];
			break;
		}
		if (input == qwertyCapital[i] && bait > 0 && gameState == "game")
		{
			--bait;
			baitedKey = qwertyValues[i];
			gremlinTimer += (gremlinTimerMax / 4);
		}
	}

	if (input == ' ')
	{
		input = '1';
	}
	if (gremlinKey == pressedKey && gremlinKey != -1 && !zoomedIn)
	{
		word[lettersGot] = input;
		++lettersGot;
		
		coins += letterValues[pressedKey] + (moreCoinsBonus * 4);
		score += letterValues[pressedKey];
	}
	if (zoomedIn && input != '1' && input != '0')
	{
		if (input == '\b' && attackWordLettersGot < 2)
			return 0;
		if (input == '\b')
		{
			attackWord[attackWordLettersGot - 1] = ' ';
			attackWordLettersGot -= 1;
			return 0;
		}
		if (input == attackWordFirstLetter && attackWordFirstLetterProtection && attackWordLettersGot == 1)
		{
			attackWordFirstLetterProtection = false;
			return input; 
		}
		attackWord[attackWordLettersGot] = input;
		++attackWordLettersGot;

		coins += static_cast<int>((letterValues[pressedKey] + (moreCoinsBonus * 4)) / 5);
		score += static_cast<int>(letterValues[pressedKey] / 5);

		if (attackWordLettersGot == 5)
		{
			printKeyboard();
			bool isRepeatWord{ false };
			for (int i{ 0 }; i <= 2; ++i)
			{
				if (attackWords[i] == attackWord)
				{
					isRepeatWord = true;
				}
			}
			if (checkIfValidWord(attackWord) && !isRepeatWord && attackTimer > 0)
			{
				gremlinKey = -1;
				setConsoleColor(consoleLightGreen);
				moveToCoordinate(keyboardWidth * 5 - 4, keyboardYStart - 1);
				for (int i{ 0 }; i <= attackWord.length(); ++i)
				{
					std::wcout << attackWord[i] << ' ';
				}
				waitMilliseconds(500);

				attackWords[attackWordsGot] = attackWord;
				++attackWordsGot;

				if (attackWordsGot == 3)
				{
					zoomedIn = false;
					runGremlinTimer = true;
					gremlinTimerDamage = false;
					clearScreen();
				}

				attackWord = L"     ";
				attackWord[0] = attackWordFirstLetter;
				attackWordLettersGot = 1;
				attackTimer = gremlinTimerMax * 3;
				attackWordFirstLetterProtection = true;
			}
			else
			{
				keyboardHealth -= 1;
				zoomedIn = false;
				runGremlinTimer = true;
				gremlinTimerDamage = false;
				attackWordFirstLetterProtection = true;
				gremlinKey = -1;
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
	if (zoomedIn && input == '1')
	{
		zoomedIn = false;
		gremlinKey = -1;
		runGremlinTimer = true;
		gremlinTimerDamage = false;
		attackWordFirstLetterProtection = true;
		clearScreen();
		word[lettersGot] = attackWordFirstLetter;
		++lettersGot;
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
		if (attackTimer >= 5000)
		{
			setConsoleColor(consoleBrightWhite);
		}
		else if (attackTimer >= 2000)
		{
			setConsoleColor(consoleLightRed);
		}
		else if (attackTimer >= 500)
		{
			setConsoleColor(consoleRed);
		}
		else if (attackTimer >= 0)
		{
			setConsoleColor(consolePurple);
		}

		moveToCoordinate(keyboardXStart + 11, keyboardYStart);
		std::wcout << L"┌────────────────────────────────────┐";
		for (int i{ 0 }; i <= 17; ++i)
		{
			moveToCoordinate(keyboardXStart + 11, keyboardYStart + i + 1);
			if (i == 0)
			{
				std::wcout << L"│ATTACK WORDS                        │";
				continue;
			}
			if (i >= 1 && i <= 3)
			{
				std::wcout << L"│-" << attackWords[i - 1] << L"                              │";
				continue;
			}
			if (i == 9)
			{
				std::wcout << L"│                 " << attackWordFirstLetter << L"                  │";
				continue;
			}
			std::wcout << L"│                                    │";
		}
		moveToCoordinate(keyboardXStart + 11, keyboardYStart + 19);
		std::wcout << L"└────────────────────────────────────┘";

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
			setConsoleColor(consoleWhite); 
		}
		else if (i == gremlinKey && gremlinTimer >= 1500)
		{
			setConsoleColor(consoleLightRed);
		}
		else if (i == gremlinKey && gremlinTimer >= 500)
		{
			setConsoleColor(consoleRed);
		}
		else if (i == gremlinKey && gremlinTimer >= 0)
		{
			setConsoleColor(consolePurple);
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

void printShop()
{
	moveToCoordinate(0, 0);
	setConsoleColor(consoleLightGreen, consoleBlack);

	std::wcout << L"SHOP\t\t\tCoins: " << coins << "\n\n";
	std::wcout << L"Item / Upgrade\tOwned\tPrice\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

	if (selectedMenuItem == 0) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "Max Health\t" << keyboardMaxHealth << '\t' << keyboardMaxHealthPrice << '\n';

	if (selectedMenuItem == 1) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "Heal\t\t" << keyboardHealth << '\t' << healPrice << '\n';

	if (selectedMenuItem == 2) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "Max Bait\t" << maxBait << '\t' << maxBaitPrice << '\n';

	if (selectedMenuItem == 3) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "Bait\t\t" << bait << '\t' << baitPrice << '\n';

	if (selectedMenuItem == 4) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "Slower Timer\t" << slowerTimerBonus << '\t' << slowerTimerPrice << '\n';

	if (selectedMenuItem == 5) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "More Coins\t" << moreCoinsBonus << '\t' << moreCoinsPrice << '\n';

	if (selectedMenuItem == 6) setConsoleColor(consoleBlack, consoleLightGreen);
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "\nPlay Again\n";

	setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "\nNavigate with Arrows, purchase with Enter.";
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

void loseState()
{
	gameState = "lose";
	moveToCoordinate(keyboardWidth * 5 - 17, keyboardYStart - 2);
	setConsoleColor(consoleLightRed);
	std::wcout << "You lose. Press space to try again.";
}

void playAgain()
{
	clearScreen();

	runGremlinTimer = true;
	gremlinTimerDamage = false;

	if (wonPrevious)
	{
		gremlinTimerMax = static_cast<int>((gremlinTimerMax * 0.9) + (slowerTimerBonus * 200));
		++gameRound;
	}
	else
	{
		gremlinTimerMax = gremlinTimerMaxRef;

		keyboardMaxHealth = 5;
		keyboardHealth = 5;
		maxBait = 3;
		bait = 3;
		slowerTimerBonus = 0;
		moreCoinsBonus = 0;

		coins = 0;
		score = 0;
		gameRound = 0;
	}

	gremlinTimer = 1000;
	attackTimer = 0;
	pressedKeyTimer = 0;

	gremlinKey = -1;
	pressedKey = -1;

	word = L"     ";
	lettersGot = 0;

	zoomedIn = false;
	attackWord = L"     ";
	attackWordLettersGot = 0;
	attackWordFirstLetterProtection = true;
	attackWordsGot = 0;

	baitedKey = -1;

	gameState = "game";
}

void printMenu()
{
	setConsoleColor(consoleLightGreen, consoleBlack);
	moveToCoordinate(0, 0);
	std::wcout << L"        ╖\n"; 
	std::wcout << L"██████  ║   ╦ ═╦═ ═╦═ ╖  ╔╗                             \n";
	std::wcout << L"██   █  ╚══ ╩  ╨   ╨  ╚═ ╚═   ▒                 ▒       \n";
	std::wcout << L"██     ████  ████ ████ ██   █▒░▒               ▒░▒      \n";
	std::wcout << L"██  ██ ██ ██ ██▄▄ ██▄▄ ████ █▒░░ ▒▒▒▒▒▒▒▒▒▒▒▒▒ ░░▒      \n";
	std::wcout << L"██   █ ████  ██▀▀ ██▀▀ ██ ███ ▒░▒   ▀▖    ▗▀  ▒░▒       \n"; 
	std::wcout << L"██████ ██ ██ ████ ████ ██  ██  ▒   ▗       ▖   ▒        \n";
	std::wcout << L"                               ▒     ▆▂▂▂▆     ▒        \n";
	std::wcout << L"▓▓  ▓▓ ▓▓▓▓▓ ▓▓   ▓▓ ▓▓▓▓   ▓▓▓▓   ▓▓▓▓  ▓▓▓▓  ▓▓▓▓     \n";
	std::wcout << L"▓▓ ▓▓  ▓▓     ▓▓ ▓▓  ▓▓ ▓▓ ▓▓  ▓▓▒▓▓  ▓▓ ▓▓ ▓▓▒▓▓ ▓▓    \n";
	std::wcout << L"▓▓▓▓   ▓▓▓▓    ▓▓    ▓▓▓▓  ▓▓  ▓▓ ▓▓▓▓▓▓▒▓▓▓▓▒ ▓▓ ▓▓    \n";
	std::wcout << L"▓▓ ▓▓  ▓▓     ▓▓     ▓▓ ▓▓ ▓▓  ▓▓ ▓▓  ▓▓ ▓▓ ▓▓ ▓▓ ▓▓    \n";
	std::wcout << L"▓▓  ▓▓ ▓▓▓▓▓ ▓▓      ▓▓▓▓   ▓▓▓▓  ▓▓  ▓▓ ▓▓ ▓▓ ▓▓▓▓     \n";
	std::wcout << L"                                                        \n";
	std::wcout << L"▒▒▒▒▒▒ ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒ ▒▒▒▒ ▒▒    ▒▒▒▒ ▒▒    ▒         \n";
	std::wcout << L"▒▒     ▒▒ ▒▒ ▒▒    ▒▒ ▒▒  ▒▒ ▒▒     ▒▒  ▒▒▒▒  ▒         \n";
	std::wcout << L"▒▒ ▒▒▒ ▒▒▒▒  ▒▒▒▒  ▒▒ ▒▒  ▒▒ ▒▒     ▒▒  ▒▒ ▒▒ ▒         \n";
	std::wcout << L"▒▒  ▒▒ ▒▒ ▒▒ ▒▒    ▒▒     ▒▒ ▒▒     ▒▒  ▒▒  ▒▒▒         \n";
	std::wcout << L"▒▒▒▒▒▒ ▒▒ ▒▒ ▒▒▒▒▒ ▒▒     ▒▒ ▒▒▒▒▒ ▒▒▒▒ ▒▒   ▒▒         \n";
	std::wcout << L"                                                        \n";


	if (selectedMenuItem == 0)
	{
		setConsoleColor(consoleBlack, consoleLightGreen);
		std::wcout << " >";
	}
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "START GAME\n";

	if (selectedMenuItem == 1)
	{
		setConsoleColor(consoleBlack, consoleLightGreen);
		std::wcout << " >";
	}
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "HELP\n";

	if (selectedMenuItem == 2)
	{
		setConsoleColor(consoleBlack, consoleLightGreen);
		std::wcout << " >";
	}
	else setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "QUIT\n";

	setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "\n\nNavigate with Arrows and Select with Enter";
	setConsoleColor(consoleGreen, consoleBlack);
	std::wcout << "\n\n\n\nMade by Kolin63";
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
	setConsoleColor(consoleLightGreen, consoleBlack);
	moveToCoordinate(70, 2);
	std::wcout << "LITTLE GREEN KEYBOARD GREMLIN";
	moveToCoordinate(70, 3);
	std::wcout << "Keyboard Health....." << keyboardHealth << "   ";
	moveToCoordinate(70, 4);
	std::wcout << "Bait................" << bait << "   ";
	moveToCoordinate(70, 5);
	std::wcout << "Coins..............." << coins << "   ";
	moveToCoordinate(70, 6);
	std::wcout << "Score..............." << score << "   ";
	moveToCoordinate(70, 7);
	std::wcout << "Round..............." << gameRound << "   ";

	// controls
	moveToCoordinate(70, 15);
	std::wcout << "HELP..................................";
	moveToCoordinate(70, 16);
	std::wcout << "Letter..............Use Letter in Word";
	moveToCoordinate(70, 17);
	std::wcout << "SHIFT + Letter......Place Bait";
	moveToCoordinate(70, 18);
	std::wcout << "SPACE...............Attempt Skip Key";
	moveToCoordinate(70, 19);
	std::wcout << "GRAVE (`)...........Exit to Menu";
}

void clearScreen()
{
	for (int i{ 0 }; i <= 29; ++i)
	{
		for (int j{ 0 }; j <= 119; ++j)
		{
			moveToCoordinate(j, i);
			std::wcout << ' ';
		}
	}
}

void printCinematic()
{
	if (selectedMenuItem == 0)
	{
		moveToCoordinate(25, 15);
		setConsoleColor(consoleLightGreen, consoleBlack);
		std::wcout << "Please press Space, then resize your window until the frame fits inside.";
	}
	if (selectedMenuItem == 1)
	{
		for (int i{ 0 }; i <= 29; ++i)
		{
			for (int j{ 0 }; j <= 119; ++j)
			{
				moveToCoordinate(j, i);
				if (i == 0 || i == 29 || j == 0 || j == 119)
				{
					std::wcout << '#';
					continue;
				}
				if (j == 46 && i == 14)
				{
					std::wcout << "    Press Space to continue.     ";
					continue;
				}
				if (j == 46 && i == 15)
				{
					std::wcout << "Or Press Enter to skip Cinematic.";
					continue;
				}
				if ((i == 14 || i == 15) && j >= 47 && j <= 79)
				{
					continue;
				}
				std::wcout << ' ';
			}
		}
		waitMilliseconds(500);
		system("CLS");
	}
	if (selectedMenuItem == 2)
	{
		moveToCoordinate(0, 0);
		std::wcout << "########################################\n";
		std::wcout << "#                                      #\n";
		std::wcout << "# ####=                            =#= #\n";
		std::wcout << "# ########       . .          ######## #\n";
		std::wcout << "# ########## :=+#%@@@%#+=   ########## #\n";
		std::wcout << "#  ############%%@@@@%%%@@%*#########  #\n";
		std::wcout << "#   #######%%@@@@@@%##@@@@@@@##**###   #\n";
		std::wcout << "#   +##@@@@%#:::#%#::::##%%@%###       #\n";
		std::wcout << "#    #@@@#: ###:::### ::#@@%+          #\n";
		std::wcout << "#    #@@@## +%+ =* *%= +#@@@%-         #\n";
		std::wcout << "#   :%@@@%##: :####- :##@@@@%*         #\n";
		std::wcout << "#   *#%@%@@@###....%##@%@@@@%*         #\n";
		std::wcout << "#  :##%#%#%@@@@%%%%%@@@@@%@@%#         #\n";
		std::wcout << "#  :##%#@%#%@@@@@%@@@@@@%#%@%#-        #\n";
		std::wcout << "#   #%%@@%@%@@@@@%@@@@%@@@%%@%.        #\n";
		std::wcout << "#   +%@%%@%#%%@@@@@@%%%%%%@@%#         #\n";
		std::wcout << "#    #%@@@@%%%%%%@%%%%%%%%%@#=         #\n";
		std::wcout << "#    +%@%%##%%%%@@@@%@@@%%%%*          #\n";
		std::wcout << "#     %%@%@@%%%@%%@%%%%%@@@#           #\n";
		std::wcout << "#     =#####%%@@@@@@@#######           #\n";
		std::wcout << "#     .#########***-.######.           #\n";
		std::wcout << "#                                      #\n";
		std::wcout << "########################################\n";	

		cinematicWrite(L"2042 AD ...\n20 years ago, a newly evolved species of Green Gremlins started \nrebellious gangs and have risen to a significant threat level. \nTheir rein stretched over the entire world, and civilization\ncollapsed under their control.", 45, 0);
		cinematicWrite(L"In an attempt to stop them, a group of scientists developed an advanced\nformula: GREmical. Theoretically, this would cause\nthe Gremlins to spontaneously combust.", 45, 6);
		cinematicWrite(L"However, an oversight caused them to not combust, but to shrink\nto a microscopic scale. Although this did cause their corrupt government\nsystem to collapse, they continued to torment humanity.", 45, 10);
		cinematicWrite(L"Their primary method of doing this is to sneak into keyboards and\ndisrupt our exchange of information. If this lasted, all scientific and\ntechnological growth would halt in its tracks.", 45, 14);
		cinematicWrite(L"Your mission is this:\nTo bravely traverse our keyboards, and abolish any Gremlins inside them.\nHowever, it is never that simple. Our communications will be active,\nand can not, for any reason, be turned off.\nYou need to make sure that every word you type while\nsmashing the Gremlins is a real one.\n\nGood Luck. You'll need it.\n\nPress Space to Continue.", 45, 18);
		selectedMenuItem = 3;
	}
	if (selectedMenuItem == 4)
	{
		gameState = "menu";
		clearScreen();
		selectedMenuItem = 0;
	}
}

void printHelp()
{
	moveToCoordinate(0, 0);
	setConsoleColor(consoleLightGreen, consoleBlack);
	std::wcout << "---OVERVIEW------------------------------------------------------------------------------------------------------------\n";
	std::wcout << "When you enter the game, you will see a to-scale replica of your keyboard. When a Gremlin runs under a key, the key    \n";
	std::wcout << "will turn red. You will have a limited amount of time to decide whether to press the key, forcing the Gremlin to run to\n";
	std::wcout << "a different one, or you can press space and try to skip that key.                                                      \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "---USING THE LETTER----------------------------------------------------------------------------------------------------\n";
	std::wcout << "If you simply press the key that the Gremlin is on, the respective letter will be added to your word (displayed at the \n";
	std::wcout << "top of the window.) Doing this means commiting to have that letter in that spot of your word for the round. By the end \n";
	std::wcout << "of the round, all five letters must make a real English word (which is decided by Wordle's word list.)                 \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "---SKIPPING THE LETTER-------------------------------------------------------------------------------------------------\n";
	std::wcout << "However, it very commonly occurs that the Gremlin will run to a key that can't possibly work with your other letters.  \n";
	std::wcout << "That is why you have the option to press space, and try to get the Gremlin off that key. To skip the letter,           \n";
	std::wcout << "you must type three 5-letter words starting with the letter of the key that you are targeting. If you do this          \n";
	std::wcout << "succesfully, the Gremlin will move to another key. If you don't, the Gremlin will still move but you will be damaged.  \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "---BAIT----------------------------------------------------------------------------------------------------------------\n";
	std::wcout << "You start the game with 3 pieces of bait. By holding SHIFT and pressing a key, you place bait on that key. This won't  \n";
	std::wcout << "cause the Gremlin to run to that key immediately, but it will run there the next time it switches keys.                \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "---STORE---------------------------------------------------------------------------------------------------------------\n";
	std::wcout << "Throughout the game, you will gain coins. You get coins from using letters, and you get more depending on how rare the \n";
	std::wcout << "letter is. If you succesfully type out a valid word, you will win the round and be taken to the store. There, you can  \n";
	std::wcout << "buy upgrades that will be active until you die. Once you die, all of your stats are reset to the default.              \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "                                                                                                                       \n";
	std::wcout << "Press Space to return to the menu.                                                                                     \n";
}

void waitMilliseconds(int x)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

void cinematicWrite(std::wstring text, int xLeft, int startY)
{
	int currentY{ startY };
	moveToCoordinate(xLeft, startY);
	for (int i{ 0 }; i <= text.length(); ++i)
	{
		std::wcout << text[i];
		if (text[i] == '\n')
		{
			++currentY;
			moveToCoordinate(xLeft, currentY);
		}
		waitMilliseconds(20);
	}
}