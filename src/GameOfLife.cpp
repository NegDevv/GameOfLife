#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace sf;

struct Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		float ms = duration.count() * 1000.0f;
		std::cout << "Time: " << ms << "ms\n";
	}
};


class Cell
{
	const int deltas[8][2] =
	{
		{0, 1}, {1, 1}, {1, 0}, {1, -1},
		{0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
	};

public:
	int x = 0, y = 0;
	bool populated = false;
	int neighbors = 0;

	void GetNeightbors(const std::vector<std::vector<Cell*>>& cells)
	{
		neighbors = 0;
		for (int i = 0; i < 8; i++)
		{
			if (((x + deltas[i][0] >= 0) && (x + deltas[i][0] < cells.size())) && ((y + deltas[i][1] >= 0) && (y + deltas[i][1] < cells[0].size())))
			{
				if (cells[x + deltas[i][0]][y + deltas[i][1]])
				{
					if (cells[x + deltas[i][0]][y + deltas[i][1]]->populated)
					{
						neighbors++;
					}
				}
			}
		}
	}

	void UpdateState()
	{
		if (populated)
		{
			if (neighbors < 2)
			{
				populated = false;
			}
			else if (neighbors > 3)
			{
				populated = false;
			}
		}
		else
		{
			if (neighbors == 3)
			{
				populated = true;
			}
		}
	}
};


void Update(const std::vector<std::vector<Cell*>> &cells)
{
	//Timer t; // Timing and printing the update time(note: doesnt include drawing and everything else that happens in the main loop, only this function)
	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[0].size(); j++)
		{
			cells[i][j]->GetNeightbors(cells);
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[0].size(); j++)
		{
			cells[i][j]->UpdateState();
		}
	}
}




void FillCells(std::vector<std::vector<Cell*>> &cells)
{
	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[0].size(); j++)
		{
			Cell *c = new Cell();
			c->x = i; c->y = j;
			c->populated = false;

			cells[i][j] = c;
		}
	}
}


void Setup(int& W, int& H)
{
	RenderWindow setup(VideoMode(500, 500), "GameOfLife");

	Font font;
	font.loadFromFile("Font/arial.ttf");
	Text promptText1;
	promptText1.setFont(font);
	promptText1.setString("Give window width in pixels:");
	promptText1.setFillColor(Color::White);
	promptText1.setCharacterSize(35);
	setup.setFramerateLimit(60);

	Text promptText2;
	promptText2.setFont(font);
	promptText2.setString("Give window height in pixels:");
	promptText2.setFillColor(Color::White);
	promptText2.setCharacterSize(35);
	setup.setFramerateLimit(60);


	Text userInputWidth;
	userInputWidth.setFont(font);
	userInputWidth.setString("");
	userInputWidth.setFillColor(Color::Red);
	userInputWidth.setCharacterSize(50);

	Text userInputHeight;
	userInputHeight.setFont(font);
	userInputHeight.setString("");
	userInputHeight.setFillColor(Color::Red);
	userInputHeight.setCharacterSize(50);

	std::string s = std::to_string(W);
	bool width = true;
	bool height = false;

	
	while (setup.isOpen())
	{
		Event e;
		while (setup.pollEvent(e))
		{
			if (e.type == Event::Closed)
			{
				setup.close();
			}

			if (e.text.unicode < 58 && e.text.unicode > 47)
			{
				if (s.length() < 4)
				{
					s += static_cast<char>(e.text.unicode);
				}
			}

			if (e.type == Event::KeyPressed)
			{
				if (e.key.code == Keyboard::Enter)
				{
					
						if (!height)
						{
							if (s.length() > 0)
							{
								W = std::stoi(s);
							}

							userInputWidth.setString(std::to_string(W));
							s.clear();
							s = std::to_string(H);
							width = false;
							height = true;
						}
						else
						{
							if (s.length() > 0)
							{
								H = std::stoi(s);
							}

							userInputWidth.setString(std::to_string(H));


							std::cout << "W: " << W << std::endl;
							std::cout << "H: " << H << std::endl;
							setup.close();
						}

				}

				if (e.key.code == Keyboard::Backspace)
				{
					if (s.length() > 0)
					{
						s.erase(s.end() - 1);
					}
					else
					{
						s.clear();
					}
				}
			}
		}

		setup.clear();

		if (width)
		{
			userInputWidth.setString(s);
		}
		else if (height)
		{
			userInputHeight.setString(s);
		}

		userInputWidth.setPosition(0, promptText1.getGlobalBounds().top + promptText1.getGlobalBounds().height);
		promptText2.setPosition(0, userInputWidth.getGlobalBounds().top + userInputWidth.getGlobalBounds().height);
		userInputHeight.setPosition(0, promptText2.getGlobalBounds().top + promptText2.getGlobalBounds().height);

		if (height)
		{
			setup.draw(promptText2);
		}

		setup.draw(promptText1);
		setup.draw(userInputWidth);
		setup.draw(userInputHeight);
		setup.display();

	}
}

int main()
{
	int windowWidth = VideoMode::getDesktopMode().width;
	int windowHeight = VideoMode::getDesktopMode().height;

	int W = windowWidth / 4, H = windowHeight / 4;

	Setup(W, H);
	

	RenderWindow app(VideoMode(W, H), "GameOfLife");
	Clock clock;
	float timer = 0;
	int tickRate = 5;
	float updateInterval = 1.0f / tickRate;
	bool paused = false;
	int updates = 0;

	Texture popTex, unpopTex;
	popTex.loadFromFile("Sprites/populated.png");
	unpopTex.loadFromFile("Sprites/unpopulated.png");


	Color Yellow(255, 255, 0, 255);
	Color Grey(100, 100, 100, 255);
	Color LightGrey(110, 110, 110, 255);

	bool YGtheme = false;


	// Yellow/grey theme. 

#define YELLOWGREY

#ifdef YELLOWGREY

	YGtheme = true;

	popTex.create(10, 10);

	int size = 10 * 10 * 4;

	Uint8* pixels = new Uint8[size];

	for (int i = 0; i < size; i += 4)
	{
		pixels[i] = 255;
		pixels[i + 1] = 255;
		pixels[i + 2] = 0;
		pixels[i + 3] = 255;
	}

	popTex.update(pixels);
	delete[] pixels;

#endif

	Sprite populated(popTex);
	Sprite unpopulated(unpopTex);

	int ppc = 10; // Pixels per cell. Default = 10(width and height)
	int columns = W / ppc;
	int rows = H / ppc;

	int zoomLevel = 1;
	Vector2f zoomCenter(0, 0);

	std::chrono::duration<float> duration;
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::high_resolution_clock::now();

	std::vector<std::vector<Cell*> > cells(columns, std::vector<Cell*>(rows, nullptr)); // 2D Vector of Cell pointers

	/*std::vector<std::vector<Cell*> > cells;
	cells.reserve(columns* rows);*/
	std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::high_resolution_clock::now();
	duration = end - start;
	float ms = duration.count() * 1000.0f;
	//std::cout << ms << "\n";

	std::chrono::time_point<std::chrono::steady_clock> totalStart, totalEnd, totalDuration;
	
	FillCells(cells);
	app.setFramerateLimit(0);


	// Main application loop
	while (app.isOpen())
	{
		totalStart = std::chrono::steady_clock::now();
		updateInterval = 1.0f / tickRate;
		float time = clock.getElapsedTime().asSeconds();
		//std::cout << time << std::endl;
		clock.restart();
		timer += time;

		Event e;
		while (app.pollEvent(e))
		{
			using std::cout;
			using std::endl;

#pragma region  INPUT

			if (e.type == Event::Closed)
			{
				app.close();
			}

			if (e.type == Event::KeyPressed)
			{
				// Press space to pause/resume simulation
				if (e.key.code == Keyboard::Space)
				{
					paused = !paused;
					if (paused)
					{
						cout << "Simulation paused!" << endl;
					}
					else
					{
						std::cout << "Simulation resumed!" << std::endl;
					}
				}
				// Increase simulation tickrate with up arrow
				else if (e.key.code == Keyboard::Up)
				{
					if (tickRate < 100)
					{
						tickRate++;
						std::cout << "Tickrate increased to: " << tickRate << std::endl;
					}
				}
				// Decrease simulation tickrate with down arrow
				else if (e.key.code == Keyboard::Down)
				{
					if (tickRate > 1)
					{
						tickRate--;
						std::cout << "Tickrate decreased to: " << tickRate << std::endl;
					}
				}

				// Clear all cells with backspace
				else if (e.key.code == Keyboard::BackSpace)
				{
					FillCells(cells);
					std::cout << "Simulation reset!" << std::endl;
					updates = 0;
					paused = true;
				}
			}

			// Zoom, currently not working properly
			/*
			if (e.type == Event::MouseWheelScrolled)
			{
				View view(FloatRect(Vector2f(0,0), Vector2f(W, H)));
				if (e.mouseWheelScroll.delta > 0)
				{
					if (zoomLevel < 10)
					{
						zoomLevel++;
						cout << "Zoom level increased!" << endl;
						cout << "Zoom level: " << zoomLevel << endl;
						//if (zoomLevel == 2)
						//{
						//	// Should set to zoom by mouse position relative to view center instead to get smooth zooming

						//	zoomCenter = Vector2f(Mouse::getPosition(app).x, Mouse::getPosition(app).y);
						//}
						//else
						//{
						    
							//zoomCenter = Vector2f(app.getView().getCenter()); // ACTUALLY PROBABLY SHOULDNT USE CENTER
							zoomCenter = Vector2f(app.getView().getCenter() + Vector2f(Mouse::getPosition(app) - Vector2i(W / 2, H / 2)) / (float)zoomLevel);
							cout << "Center x: " << app.getView().getCenter().x << std::endl;
							cout << "Center y: " << app.getView().getCenter().y << std::endl;
						//}
					}
				}
				else
				{
					if (zoomLevel > 1)
					{
						zoomLevel--;
						cout << "Zoom level decreased!" << endl;
						cout << "Zoom level: " << zoomLevel << endl;
						zoomCenter = Vector2f(app.getView().getCenter() - Vector2f(Mouse::getPosition(app) - Vector2i(W / 2, H / 2)) / (float)zoomLevel);
					}
					if (zoomLevel == 1)
					{
						zoomCenter = Vector2f(W / 2, H / 2);
					}
				}

				view.setCenter(zoomCenter);
				view.setSize(W / zoomLevel, H / zoomLevel);
				app.setView(view);
			}
			*/
		}

		// Populate cell with left mouse button
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			int x = Mouse::getPosition(app).x / ppc;
			int y = Mouse::getPosition(app).y / ppc;

			if(x >= 0 && x < columns && y >= 0 && y < rows)
			{
				// Depopulate cell with left shift pressed
				if (Keyboard::isKeyPressed(Keyboard::LShift))
				{
					cells[x][y]->populated = false;
				}
				else
				{
					cells[x][y]->populated = true;
					/*cells[x+1][y]->populated = true; // IMPORTANT: If you increase the draw size implement a check that checks that additional cells are inside the array
					cells[x+1][y+1]->populated = true;	// Problem at the borders
					cells[x][y+1]->populated = true;*/
				}
			}
		}

		// Depopulate cell with right mouse button
		if (Mouse::isButtonPressed(Mouse::Right))
		{
			int x = Mouse::getPosition(app).x / ppc;
			int y = Mouse::getPosition(app).y / ppc;

			if (x >= 0 && x < columns && y >= 0 && y < rows)
			{
				cells[x][y]->populated = false;
			}
		}

		//std::cout << "Mouse x: " << Mouse::getPosition(app).x << std::endl;
		//std::cout << "Mouse y: " << Mouse::getPosition(app).y << std::endl;

#pragma endregion

		// Simulation update
		if (timer > updateInterval && !paused)
		{
			timer = 0;
			updates++;
			std::cout << "Update no: " << updates << "\n";
			Update(cells);
		}

		// Draw
		{
			// TODO: Optimization: only redraw changed cells


			app.clear();

			for (int i = 0; i < cells.size(); i++)
			{
				for (int j = 0; j < cells[0].size(); j++)
				{
					if (cells[i][j]->populated)
					{
						populated.setPosition(i * ppc, j * ppc);
						app.draw(populated);
					}
					else
					{
						unpopulated.setPosition(i * ppc, j * ppc);
						if (i % 2 == 0)
						{
							if (j % 2 == 0)
							{
								if (YGtheme)
								{
									unpopulated.setColor(LightGrey);
								}
								else
								{
									unpopulated.setColor(Color::Color(240, 240, 240, 255));
								}
							}
							else
							{
								if (YGtheme)
								{
									unpopulated.setColor(Grey);
								}
								else
								{
									unpopulated.setColor(Color::White);
								}
							}
						}
						else
						{
							if (YGtheme)
							{
								unpopulated.setColor(Grey);
							}
							else
							{
								unpopulated.setColor(Color::White);
							}
						}
						app.draw(unpopulated);
					}
				}
			}
			app.display();
		}
			
		totalEnd = std::chrono::steady_clock::now();
		//using fmilli = std::chrono::duration<double, std::milli>;
		//std::cout << "Total update duration: " << fmilli(totalEnd - totalStart).count() << "ms\n";
	}

	return 0;
}