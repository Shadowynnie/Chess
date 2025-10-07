#include "GameManager.h"


/* TODO: Handle input events, update game state*/
// Update game state (e.g., check for win conditions, update figure positions)
enum class GameState
{
	MAIN_MENU,
	HOST_GAME,
	CONNECT_TO_GAME,
	PLAYING,
	GAME_OVER,
	CLOSED
};

enum class PieceType 
{
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
	PAWN
};

GameManager::GameManager() : _tiles{}, _playerOneFigures{}, _playerTwoFigures{}
{}

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
		{ PieceType::ROOK,   [](int x,int y,bool w) { return new Rook(x,y,w); } },
		{ PieceType::KNIGHT, [](int x,int y,bool w) { return new Knight(x,y,w); } },
		{ PieceType::BISHOP, [](int x,int y,bool w) { return new Bishop(x,y,w); } },
		{ PieceType::QUEEN,  [](int x,int y,bool w) { return new Queen(x,y,w); } },
		{ PieceType::KING,   [](int x,int y,bool w) { return new King(x,y,w); } },
		{ PieceType::PAWN,   [](int x,int y,bool w) { return new Pawn(x,y,w); } },
	};

	// Back rank layout
	std::array<PieceType, 8> backRank = {
		PieceType::ROOK, PieceType::KNIGHT, PieceType::BISHOP, PieceType::QUEEN,
		PieceType::KING, PieceType::BISHOP, PieceType::KNIGHT, PieceType::ROOK
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
			Figure* fig = factories[PieceType::PAWN](file, pawnRow, isWhite);
			figures.push_back(fig);
			_tiles[file][pawnRow].SetFigure(true);
		}
	};

	initPlayer(true);   // White
	initPlayer(false);  // Black
}

void GameManager::Draw(sf::RenderWindow& window, GameState gameState) 
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
	/*Handle input events (clicking on a figure, etc)*/
}

void GameManager::Update() 
{
	// This function can be expanded to include game logic
	sf::RenderWindow window(sf::VideoMode({ 1024,1024 }, sf::VideoMode::getDesktopMode().bitsPerPixel), "Chess Game", sf::Style::Close | sf::Style::Resize);

    // MAIN GAME LOOP
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

// Display main menu (host game, join game, quit)
void GameManager::MainMenu() 
{
	
}

// Reset game state to initial conditions
void GameManager::ResetGame() 
{
	_playerOneFigures.clear();
	_playerTwoFigures.clear();
	InitializeBoard();
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
