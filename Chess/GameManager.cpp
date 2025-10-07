#include "GameManager.h"


enum class PieceType { Rook, Knight, Bishop, Queen, King, Pawn };

GameManager::GameManager() 
{
	InitializeBoard();
}
GameManager::~GameManager() 
{
	// Clean up dynamically allocated memory for figures
	for (auto figure : _playerOneFigures) 
	{
		delete figure;
	}
	for (auto figure : _playerTwoFigures) 
	{
		delete figure;
	}
}

void GameManager::InitializeBoard()
{
	// Load all textures at the start
	AssetManager::LoadTextures();

	// Initialize tiles
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			_tiles[i][j] = Tile(i, j, false);
		}
	}

	// Factory map for all piece types, this can replace switch-case or if-else chains
	map<PieceType, function<Figure* (int, int, bool)>> factories = {
		{ PieceType::Rook,   [](int x,int y,bool w) { return new Rook(x,y,w); } },
		{ PieceType::Knight, [](int x,int y,bool w) { return new Knight(x,y,w); } },
		{ PieceType::Bishop, [](int x,int y,bool w) { return new Bishop(x,y,w); } },
		{ PieceType::Queen,  [](int x,int y,bool w) { return new Queen(x,y,w); } },
		{ PieceType::King,   [](int x,int y,bool w) { return new King(x,y,w); } },
		{ PieceType::Pawn,   [](int x,int y,bool w) { return new Pawn(x,y,w); } },
	};

	// Back rank layout
	std::array<PieceType, 8> backRank = {
		PieceType::Rook, PieceType::Knight, PieceType::Bishop, PieceType::Queen,
		PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook
	};

	// Helper lambda for player initialization
	auto initPlayer = [&](bool isWhite)
	{
		int backRankRow = isWhite ? 0 : 7;
		int pawnRow = isWhite ? 1 : 6;
		auto& figures = isWhite ? _playerOneFigures : _playerTwoFigures;

		// Back rank
		for (int file = 0; file < 8; ++file)
		{
			PieceType type = backRank[file];
			Figure* fig = factories[type](file, backRankRow, isWhite);
			figures.push_back(fig);
			_tiles[file][backRankRow].SetFigure(true);
		}

		// Pawns
		for (int file = 0; file < 8; ++file)
		{
			Figure* fig = factories[PieceType::Pawn](file, pawnRow, isWhite);
			figures.push_back(fig);
			_tiles[file][pawnRow].SetFigure(true);
		}
	};

	initPlayer(true);   // White
	initPlayer(false);  // Black
}

void GameManager::Draw(sf::RenderWindow& window) 
{
	// Draw tiles
	for (int i = 0; i < 8; ++i) 
	{
		for (int j = 0; j < 8; ++j) 
		{
			sf::RectangleShape tileShape(sf::Vector2f(128, 128));
			tileShape.setPosition(sf::Vector2f(float(i * 128), float(j * 128)));
			tileShape.setFillColor((i + j) % 2 == 0 ? sf::Color(118, 150, 86) : sf::Color(238, 238, 210));
			window.draw(tileShape);
		}
	}
	// Draw figures
	for (auto figure : _playerOneFigures) 
	{
		window.draw(figure->GetSprite());
	}
	for (auto figure : _playerTwoFigures) 
	{
		window.draw(figure->GetSprite());
	}
}

void GameManager::HandleInput(sf::Event event) 
{
	/*TODO: Handle input events (clicking on a figure, etc)*/
}

void GameManager::Update() 
{
	/* TODO: Handle input events, update game state*/
	// Update game state (e.g., check for win conditions, update figure positions)
	// This function can be expanded to include game logic
	sf::RenderWindow window(sf::VideoMode({ 1024,1024 }, sf::VideoMode::getDesktopMode().bitsPerPixel), "Chess Game", sf::Style::Close | sf::Style::Resize);

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

/* // Debug draw with red circles on tiles that have figures
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

			// Debug red circle if hasFigure == true
			if (tiles[i][j].hasFigure)
			{
				sf::CircleShape circle(25); // radius = 25px
				circle.setFillColor(sf::Color::Red);
				circle.setOrigin(sf::Vector2f(float(circle.getRadius()), float(circle.getRadius())));
				circle.setPosition(sf::Vector2f(float(i * 64 + 32), float(j * 64 + 32)));
				window.draw(circle);
			}
		}
	}

	// Draw figures
	for (auto figure : player1figures) window.draw(figure->getSprite());
	for (auto figure : player2figures) window.draw(figure->getSprite());
}*/
