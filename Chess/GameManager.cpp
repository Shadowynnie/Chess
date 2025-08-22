#include "GameManager.h"

GameManager::GameManager() 
{
	InitializeBoard();
}
GameManager::~GameManager() 
{
	// Clean up dynamically allocated memory for figures
	for (auto figure : player1figures) 
	{
		delete figure;
	}
	for (auto figure : player2figures) 
	{
		delete figure;
	}
}
void GameManager::InitializeBoard() 
{
	// Initialize tiles
	for (int i = 0; i < 8; ++i) 
	{
		for (int j = 0; j < 8; ++j) 
		{
			tiles[i][j] = Tile(i, j, false);
		}
	}

	// Initialize player 1 figures (white)
	player1figures.push_back(new Rook(0, 0, true));
	player1figures.push_back(new Knight(1, 0, true));
	player1figures.push_back(new Bishop(2, 0, true));
	player1figures.push_back(new Queen(3, 0, true));
	player1figures.push_back(new King(4, 0, true));
	player1figures.push_back(new Bishop(5, 0, true));
	player1figures.push_back(new Knight(6, 0, true));
	player1figures.push_back(new Rook(7, 0, true));
	for (int i = 0; i < 8; ++i) 
	{
		player1figures.push_back(new Pawn(i, 1, true));
	}

	// Initialize player 2 figures (black)
	player2figures.push_back(new Rook(0, 7, false));
	player2figures.push_back(new Knight(1, 7, false));
	player2figures.push_back(new Bishop(2, 7, false));
	player2figures.push_back(new Queen(4, 7, false));
	player2figures.push_back(new King(3, 7, false));
	player2figures.push_back(new Bishop(5, 7, false));
	player2figures.push_back(new Knight(6, 7, false));
	player2figures.push_back(new Rook(7, 7, false));
	for (int i = 0; i < 8; ++i) 
	{
		player2figures.push_back(new Pawn(i, 6, false));
	}
	
	// Set hasFigure for tiles with figures
	for (auto figure : player1figures) 
	{
        int fx = figure->getSprite().getPosition().x / 64;
        int fy = figure->getSprite().getPosition().y / 64;
        if (fx >= 0 && fx < 8 && fy >= 0 && fy < 8) 
		{
            tiles[fx][fy].hasFigure = true;
        }
	}
}

void GameManager::Draw(sf::RenderWindow& window) 
{
	// Draw tiles
	for (int i = 0; i < 8; ++i) 
	{
		for (int j = 0; j < 8; ++j) 
		{
			sf::RectangleShape tileShape(sf::Vector2f(64, 64));
			tileShape.setPosition(sf::Vector2f(float(i * 64), float(j * 64)));
			tileShape.setFillColor((i + j) % 2 == 0 ? sf::Color(118, 150, 86) : sf::Color(238, 238, 210));
			window.draw(tileShape);
		}
	}
	// Draw figures
	for (auto figure : player1figures) 
	{
		window.draw(figure->getSprite());
	}
	for (auto figure : player2figures) 
	{
		window.draw(figure->getSprite());
	}
}

void GameManager::HandleInput(sf::Event event) 
{
	// Handle user input events (e.g., mouse clicks, key presses)
	// This function can be expanded to handle specific game logic
}

void GameManager::Update() 
{
	
	// Update game state (e.g., check for win conditions, update figure positions)
	// This function can be expanded to include game logic
	sf::RenderWindow window(sf::VideoMode({ 512,512 }, sf::VideoMode::getDesktopMode().bitsPerPixel), "Chess Game", sf::Style::Close | sf::Style::Resize);

	while (window.isOpen())
	{

		std::optional<sf::Event> event;
		while (event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() ||
				(event->is<sf::Event::KeyPressed>() &&
					event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
			{
				window.close();
			}
		}
		window.clear();
		Draw(window);


		window.display();

	}
}