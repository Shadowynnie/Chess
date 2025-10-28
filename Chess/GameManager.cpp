#include "GameManager.h"
#include "Colors.h"

#include "Figurines/Figure.h"
#include "Figurines/Bishop.h"
#include "Figurines/King.h"
#include "Figurines/Knight.h"
#include "Figurines/Rook.h"
#include "Figurines/Queen.h"
#include "Figurines/Pawn.h"

/* TODO: Add AI player bot for singleplayer mode
* Implement networking for multiplayer mode
* Handle checkmate and stalemate conditions
* Add en passant ability to Pawn
* Add the option to promote Pawn to another piece
*/

static bool _currentRound = true; // True for player 1's turn, false for player 2's turn
GameState _currentState;
static Tile _tiles[8][8]; // 2D array of tiles representing the chessboard
static vector<Figure*> _playerOneFigures; // Vector to hold player 1 chess pieces (WHITE)
static vector<Figure*> _playerTwoFigures; // Vector to hold player 2 chess pieces (BLACK)
sf::RenderWindow _window(sf::VideoMode({ 1024,1024 }, sf::VideoMode::getDesktopMode().bitsPerPixel), "Chess Game", sf::Style::Close | sf::Style::Resize);

// Replaces switch-case or if-else chains for state management
map<GameState, function<void()>> stateFunctions
{
	{ GameState::MAIN_MENU, [&]() { GameManager::MainMenu(); }},
	{ GameState::HOST_GAME, []() { GameManager::HostGame(); }},
	{ GameState::CONNECT_TO_GAME, []() { GameManager::ConnectToGame(); }},
	{ GameState::SINGLEPLAYER, []() { GameManager::PlayGame(); }},
	{ GameState::GAME_OVER, []() { GameManager::DeinitializeBoard(); }},
	{ GameState::CLOSED, [&]() { _window.close(); } }
};

// Initialize the chessboard with tiles and figures for both players
void GameManager::InitializeBoard()
{
	// Initialize tiles
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			_tiles[i][j] = Tile(i, j, false);
		}
	}

	// Factory map for all piece types
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
			_tiles[file][backRankRow].SetFigure(fig);
        }
		// Pawns
		for (int file = 0; file < 8; ++file)
		{
			Figure* fig = factories[PieceType::PAWN](file, pawnRow, isWhite);
			figures.push_back(fig);
			_tiles[file][pawnRow].SetFigure(fig);
        }
	};

	initPlayer(true);   // White
	initPlayer(false);  // Black
    cout << "Board initialized with pieces." << endl;
}

void GameManager::DeinitializeBoard()
{
	// Clean up dynamically allocated memory for figures
	for (auto& figures : { _playerOneFigures, _playerTwoFigures })
	{
		for (auto figure : figures)
			delete figure;
	}
	cout << "Board deinitialized and memory cleaned up." << endl;
    _playerOneFigures.clear();
    _playerTwoFigures.clear();
}

// Drawing function definitions
// Draw the chessboard tiles
void GameManager::DrawTiles(sf::RenderWindow& window, Tile tiles[8][8])
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			Tile& tile = tiles[i][j];

            // if the tile is not in check, use standard colors
			if (!tile.IsInCheck())
			{
				tile.TileShape.setFillColor((i + j) % 2 == 0
					? Colors::LightTile // light beige
					: Colors::DarkTile);// dark green
			}
			else
				tile.TileShape.setFillColor(Colors::InCheckTile); // red color for check

			tile.TileShape.setPosition(sf::Vector2f(float(i * 128), float(j * 128)));
			window.draw(tile.TileShape);
		}
	}
}

// Draw the chess pieces for both players
void GameManager::DrawFigures(sf::RenderWindow& window,
	const std::vector<Figure*>& white,
	const std::vector<Figure*>& black)
{
	auto drawSide = [&](const std::vector<Figure*>& figures)
	{
		for (auto figure : figures)
		{
			if (figure)
				window.draw(figure->GetSprite());
		}
	};
	drawSide(white);
	drawSide(black);
}

// Draw highlighted tiles
void GameManager::DrawHighlights(sf::RenderWindow& window, Tile tiles[8][8])
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			Tile& tile = tiles[i][j];
			if (tile.IsHighlighted())
			{
				tile.HighLight.setFillColor(Colors::AvailableMove);
				tile.HighLight.setPosition(sf::Vector2f(
					float(tile.GetX() * 128 + 44),
					float(tile.GetY() * 128 + 44)
				));
				window.draw(tile.HighLight);
			}
		}
	}
}

void GameManager::DrawGame()
{
	// Tiles
	DrawTiles(_window, _tiles);

	// Figures
	DrawFigures(_window, _playerOneFigures, _playerTwoFigures);

	// Highlights
	DrawHighlights(_window, _tiles);
}

void GameManager::ClearHighlitghts()
{
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
			_tiles[i][j].Highlight(false);
	}
}

void GameManager::CheckForCheck()
{
	// Check if either king is in check
	auto checkKing = [&](bool isWhiteKing)
	{
		Tile* kingTile = nullptr;
		auto& friendlyFigures = isWhiteKing ? _playerOneFigures : _playerTwoFigures;
		auto& enemyFigures = isWhiteKing ? _playerTwoFigures : _playerOneFigures;
		// Find the king's tile
		for (auto figure : friendlyFigures)
		{
			if (typeid(*figure) == typeid(King))
			{
				kingTile = figure->GetCurrentTile(_tiles);
				break;
			}
		}
		if (kingTile)
		{
			King* king = dynamic_cast<King*>(kingTile->GetFigure());
			if (king && king->IsThreatened(_tiles, enemyFigures))
			{
				kingTile->SetInCheck(true);
				cout << (isWhiteKing ? "White" : "Black") << " King is in check!" << endl;
			}
			else
				kingTile->SetInCheck(false);
		}
	};
	checkKing(true);  // Check white king
    checkKing(false); // Check black king
}

void GameManager::Update() 
{
	std::optional<sf::Event> event;
	sf::Vector2i mousePos;
	Tile* selectedTile = nullptr;
    Tile* previousSelectedTile = nullptr;

    // GAME LOOP
	while (_window.isOpen())
	{
		_window.clear();        
        DrawGame();
        // Handle input events
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
                DeinitializeBoard();
				_window.close();
			}
			if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				_currentState = GameState::MAIN_MENU;
                stateFunctions[_currentState]();
			}
			if (event->is<sf::Event::MouseButtonPressed>() && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
			{
				mousePos = sf::Mouse::getPosition(_window);
                int tileX = mousePos.x / 128;
                int tileY = mousePos.y / 128;

                cout << "Mouse clicked at tile X: " << tileX << " Y: " << tileY << endl;

				if (tileX >= 0 && tileX < 8 && tileY >= 0 && tileY < 8)
				{
					selectedTile = &_tiles[tileX][tileY];

                    // Debug information
                    cout << "-------------------------" << endl;
                    cout << "Selected Tile - X: " << selectedTile->GetX() << " Y: " << selectedTile->GetY() << endl;
                    cout << "Is tile occupied?" << (selectedTile->IsOccupied() ? " Yes" : " No") << endl;
					cout << "Is previous tile nullptr?" << (previousSelectedTile == nullptr ? " Yes" : " No") << endl;
					cout << "Is tile highlighted?" << (selectedTile->IsHighlighted() ? " Yes" : " No") << endl;
                    cout << "Current round (true=white, false=black): " << (_currentRound ? "White" : "Black") << endl;
                    cout << "Tile's figure pointer? " << (selectedTile->GetFigure() != nullptr ? " Yes" : " No") << endl;
					cout << "Figure color on selected tile: " << (selectedTile->IsOccupied() ? (selectedTile->GetFigure()->GetColor() ? "White" : "Black") : "N/A") << endl;
                    cout << "Figure position on selected tile: " << (selectedTile->IsOccupied() ? ("X: " + std::to_string(selectedTile->GetFigure()->GetX()) + " Y: " + std::to_string(selectedTile->GetFigure()->GetY())) : "N/A") << endl;
                    cout << "Is tile red? " << (selectedTile->IsInCheck() ? " Yes" : " No") << endl;
                    cout << "Is it a Rook? " << (selectedTile->IsOccupied() ? (typeid(*selectedTile->GetFigure()) == typeid(Rook) ? " Yes" : " No") : " N/A") << " And did it move? " << (selectedTile->IsOccupied() ? (typeid(*selectedTile->GetFigure()) == typeid(Rook) ? (dynamic_cast<Rook*>(selectedTile->GetFigure())->HasMoved() ? " Yes" : " No") : " N/A") : " N/A") << endl;

                    // When the player clicks on a highlighted tile to move
					if (previousSelectedTile != nullptr)
					{
						if ((selectedTile->IsHighlighted()) && (previousSelectedTile->GetFigure()->GetColor() == _currentRound))
						{
							if (selectedTile->IsOccupied())
                            {	// Capture opponent's piece
								if (selectedTile->GetFigure()->GetColor() != _currentRound)
									previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, selectedTile->GetFigure()->GetColor() != true ? _playerTwoFigures : _playerOneFigures);
							}
							else
								previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile,_tiles);
							ClearHighlitghts();
							// Switch turns
							_currentRound = !_currentRound;
							previousSelectedTile = nullptr;
						}
                    }
					// When the player clicks on a figure for the first time
					if ((selectedTile->IsOccupied()) && (selectedTile->GetFigure()->GetColor() == _currentRound))
					{
						vector<Tile*> possibleMoves;
						if (typeid(*selectedTile->GetFigure()) == typeid(King))
						{
							auto& enemyFigures = _currentRound ? _playerTwoFigures : _playerOneFigures;
                            possibleMoves = selectedTile->GetFigure()->GetPossibleMoves(_tiles, enemyFigures);
						}
						else
							possibleMoves = selectedTile->GetFigure()->GetPossibleMoves(_tiles);
                        
                        ClearHighlitghts();
                        selectedTile->GetFigure()->HighlightPossibleMoves(possibleMoves);
						previousSelectedTile = selectedTile;
					}
					CheckForCheck();
                }
			}
		}
		_window.display();
	}
}

void GameManager::MainMenu()
{
	_window.setVerticalSyncEnabled(true);
    // Buttons
    sf::Sprite singleplayerButton = AssetManager::GetSprite("menu_singleplayer");
    singleplayerButton.setPosition(sf::Vector2f(362.f, 150.f));
	sf::Sprite hostButton = AssetManager::GetSprite("menu_host");
	hostButton.setPosition(sf::Vector2f(362.f, 300.f));
	sf::Sprite joinButton = AssetManager::GetSprite("menu_join");
	joinButton.setPosition(sf::Vector2f(362.f, 450.f));
	sf::Sprite quitButton = AssetManager::GetSprite("menu_quit");
	quitButton.setPosition(sf::Vector2f(362.f, 600.f));

	std::optional<sf::Event> event;
	sf::Vector2i mousePos;

	// MAIN MENU LOOP
	while (_window.isOpen())
	{
		
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				_window.close();
			// Handle mouse button press events
			if (event->is<sf::Event::MouseButtonPressed>() &&
				(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
			{
				mousePos = sf::Mouse::getPosition(_window);
				if (singleplayerButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Single player -> player vs computer." << endl;
					// Transition to singleplayer game state
                    _currentState = GameState::SINGLEPLAYER;
				}
				if (hostButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Host Game button clicked!" << endl;
					// Transition to host game state
                    _currentState = GameState::HOST_GAME;
				}
				else if (joinButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Join Game button clicked!" << endl;
					// Transition to join game state
                    _currentState = GameState::CONNECT_TO_GAME;
				}
				else if (quitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Quit button clicked!" << endl;
					// Transition to closed state
                    _currentState = GameState::CLOSED;
				}
                // Switch to the selected state and execute its function
                stateFunctions[_currentState]();
			}
		}
		_window.clear();
        _window.draw(singleplayerButton);
		_window.draw(hostButton);
		_window.draw(joinButton);
		_window.draw(quitButton);
		_window.display();
	}
}

void GameManager::HostGame()
{
	cout << "Hosting a Game..." << endl;
	// Placeholder for hosting game logic
}

void GameManager::ConnectToGame()
{
	cout << "Connecting to a Game..." << endl;
	// Placeholder for connecting to game logic
}

void GameManager::PlayGame()
{
	cout << "Starting the Game..." << endl;
	// Placeholder for game loop logic
	InitializeBoard();
    Update();
}