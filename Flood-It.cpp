#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;
using namespace sf;

// Predefined global constants 

#define gwidth 14
#define gheight 14
#define leftover 0.12
#define WinW (VideoMode::getDesktopMode().width)
#define WinH (VideoMode::getDesktopMode().height)
#define WinWRatio 0.45 //ratio of the screen width
#define WinHRatio (0.45 * WinW / WinH)
#define maxn int(gheight + (3 / 2.0) * gwidth)

//By Default: the game window(inner content) is a square(porportional to the screen size to make it "available" for any resolution); the video mode size can be, however, changed to any numeric values here:
#define defaultGameW (WinWRatio * WinW)
#define defaultGameH (WinHRatio * WinH)
#define fontSize 16

VideoMode defaultMode(defaultGameW, defaultGameH);

Color backgroundColor(33, 66, 99);
const double tileWidth = (1.0 - 2.0 * leftover) * double(defaultMode.width) / double(gwidth); 			//automatic scaling is guranteed; not affected by resizing
const double tileHeight = (1.0 - 2.0 * leftover) * double(defaultMode.height) / double(gheight);
const Vector2f tileSize(tileWidth, tileHeight);


const Color preset[8] = { Color::Black, Color::White, Color::Red, Color::Blue, Color::Yellow, Color::Green, Color::Magenta, Color::Cyan }; 	//random from 0 to 7


// The used functions' prototypes and definitions

Vector2f indicesToPos(int i, int j) 	 //converts i, j to x, y positions, respectively
{
	Vector2u size(defaultMode.width, defaultMode.height);
	Vector2f wpos(leftover * size.x, leftover * size.y);
	wpos += Vector2f(i * tileWidth, j * tileHeight);

	return wpos;
}

Vector2i posToIndex(int x, int y)   	//converts x, y to i, j resp.
{
	Vector2f origin = indicesToPos(0, 0);
	Vector2f pos(x, y);
	pos = pos - origin;
	pos.x = floor(pos.x / tileWidth);
	pos.y = floor(pos.y / tileHeight);

	return Vector2i(pos);
}

void initializeGrid(Color gridColor[][gheight], RectangleShape grid[][gheight])  //to initialize the grid
{
	for (int i = 0; i < gwidth; i++)
	{
		for (int j = 0; j < gheight; j++)
		{
			gridColor[i][j] = preset[rand() % 8];
			grid[i][j].setFillColor(gridColor[i][j]);
		}
	}
}

bool colorExist(Color gridColors[][gheight], Color chosenColor)      // checks if the chosen color exists in the grid
{
	for (int i = 0; i < gwidth; i++)
	{
		for (int j = 0; j < gheight; j++)
		{
			if (gridColors[i][j] == chosenColor) return true;
		}
	}
	return false;
}

void change(Color chosenColor, Color gridColors[][gheight], Color oldColor, int i = 0, int j = 0)        // to change the colors of the grid
{
	if (oldColor == chosenColor) return;

	if (gridColors[i][j] == oldColor) gridColors[i][j] = chosenColor;

	if (gridColors[i + 1][j] == oldColor && i + 1 <= gwidth - 1) change(chosenColor, gridColors, oldColor, i + 1, j);

	if (gridColors[i][j + 1] == oldColor && j + 1 <= gheight - 1) change(chosenColor, gridColors, oldColor, i, j + 1);

	if (gridColors[i - 1][j] == oldColor && i - 1 >= 0) change(chosenColor, gridColors, oldColor, i - 1, j);

	if (gridColors[i][j - 1] == oldColor && j - 1 >= 0) change(chosenColor, gridColors, oldColor, i, j - 1);
}

bool isWon(Color gridColors[][gheight]) 			// to check if the game is won
{
	for (int i = 0; i < gwidth; i++)
	{
		for (int j = 0; j < gheight; j++)
		{
			if (gridColors[0][0] != gridColors[i][j]) return false;
		}
	}
	return true;
}

void render(RenderWindow& window, RectangleShape grid[][gheight], Color gridColors[][gheight], Text &counter, Text& result, Text &instructions)  // to display
{
	window.clear(backgroundColor);
	for (int i = 0; i < gwidth; i++)
	{
		for (int j = 0; j < gheight; j++)
		{
			grid[i][j].setFillColor(gridColors[i][j]);
			grid[i][j].setSize(tileSize);
			grid[i][j].setPosition(indicesToPos(i, j));
			window.draw(grid[i][j]);
		}
	}
	window.draw(counter);
	window.draw(result);
	window.draw(instructions);
	window.display();
}

int main()
{
	srand(time(NULL));

	// game controls and extras

	int count = 0;
	bool flag = false;
	int r, p, g;

	// game objects

	RenderWindow mainWindow(defaultMode, "Flood-It");
	RectangleShape grid[gwidth][gheight];
	Color gridColors[gwidth][gheight];
	Event event;
	Vector2i mousePos;

	// starting the game

	initializeGrid(gridColors, grid);

	// Texts and fonts

	Text counter;
	Text result;
	Text instructions;
	Font arial;
	Font cool;
	if (!cool.loadFromFile("GoodDog.otf") || !arial.loadFromFile("arial.ttf"))
	{
		cout << "font can not be found" << endl;
	}
	counter.setFont(cool);
	result.setFont(cool);
	instructions.setFont(arial);
	counter.setCharacterSize(40);
	result.setCharacterSize(80);
	instructions.setCharacterSize(15);
	counter.setPosition(indicesToPos(gwidth / 2.0 - 1.0, gheight));
	instructions.setPosition(0, 0);
	instructions.setString(" Use the mouse or the keybord to flood the grid with only one color within 35 steps \n (B = Blue, X = Black, W = White, Y = Yellow, R = Red, G = Green, M = Magenta, C = Cyan) \n Space = restart at any time, Escape = close");

	// Game Loop	

	while (mainWindow.isOpen())
	{
		Color chosenColor = gridColors[0][0];

		string btngan = to_string(count) + "/" + to_string(maxn);       // counter
		counter.setString(btngan);

		while (mainWindow.pollEvent(event))        // Events loop
		{
			switch (event.type)      //enums in sf::Event
			{
			case Event::Closed:
				mainWindow.close();
				break;

			case Event::MouseButtonReleased:			// choosing colors using the mouse
				mousePos = posToIndex(event.mouseButton.x, event.mouseButton.y);
				if (!(mousePos.x > gwidth-1) && !(mousePos.x < 0) && !(mousePos.y > gheight-1) && !(mousePos.y < 0))
					chosenColor = gridColors[mousePos.x][mousePos.y];
				break;

			case Event::KeyPressed:                     // choosing colors using the keyboard
				switch (event.key.code)
				{
				case Keyboard::B:
					chosenColor = Color::Blue;
					break;
				case Keyboard::W:
					chosenColor = Color::White;
					break;
				case Keyboard::Y:
					chosenColor = Color::Yellow;
					break;
				case Keyboard::C:
					chosenColor = Color::Cyan;
					break;
				case Keyboard::G:
					chosenColor = Color::Green;
					break;
				case Keyboard::M:
					chosenColor = Color::Magenta;
					break;
				case Keyboard::R:
					chosenColor = Color::Red;
					break;
				case Keyboard::X:
					chosenColor = Color::Black;
					break;
				case Keyboard::Escape:
					mainWindow.close();
					break;
				case Keyboard::Space:
					mainWindow.close();
					main();
					break;
				}
				break;
			}
			
			if (!flag && count < maxn && chosenColor != gridColors[0][0] && colorExist(gridColors, chosenColor))        // change the colors after checking the counter and the existing colors
			{
				change(chosenColor, gridColors, gridColors[0][0]);
				count++;
			}
			flag = isWon(gridColors);			
		}
		
		if (count == maxn && !flag) 				// To check if the player lost 
		{
			result.setString("Game Over!");
			result.setPosition(indicesToPos(gwidth / 2.0 - 4.0, gheight / 2.0 - 2.0));
			result.setFillColor(backgroundColor);			
		}
		
		if (flag)									// To check if the player won
		{
			result.setString("You WIN!!");
			result.setPosition(indicesToPos(gwidth / 2.0 - 4.0, gheight / 2.0 - 2.0));
			r = rand() % 256;
			g = rand() % 256;
			p = rand() % 256;
			result.setFillColor(Color(p, g, r));
			chosenColor = Color(r, p, g);
			change(chosenColor, gridColors, gridColors[0][0]);
			sleep(milliseconds(100));
		}
		
		render(mainWindow, grid, gridColors, counter, result, instructions);	   // displays everything
	}
	
	return 0;
}
