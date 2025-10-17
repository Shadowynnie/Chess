#include "GameManager.h"
/* TODO: Handle input events, update game state*/
// Update game state (e.g., check for win conditions, update figure positions)


static bool _currentRound = true; // True for player 1's turn, false for player 2's turn
GameState _currentState;
static Tile _tiles[8][8]; // 2D array of tiles representing the chessboard
static vector<Figure*> _playerOneFigures; // Vector to hold player 1 chess pieces
static vector<Figure*> _playerTwoFigures; // Vector to hold player 2 chess pieces
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

void GameManager::DrawGame()
{
	// Draw tiles
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			_tiles[i][j].TileShape.setPosition(sf::Vector2f(float(i * 128), float(j * 128)));
            _tiles[i][j].TileShape.setFillColor((i + j) % 2 == 0 ? sf::Color(118, 150, 86) : sf::Color(238, 238, 210));

			if (_tiles[i][j].IsHighlighted())
			{
				_tiles[i][j].HighLight.setFillColor(sf::Color(255, 0, 0, 128)); // Semi-transparent red
				_tiles[i][j].HighLight.setPosition(sf::Vector2f(_tiles[i][j].GetX() * 128 + 44, _tiles[i][j].GetY() * 128 + 44));
				_window.draw(_tiles[i][j].TileShape);
				_window.draw(_tiles[i][j].HighLight);
			}
			else
				_window.draw(_tiles[i][j].TileShape);
		}
	}
	// Draw figures
	for (auto& figures : { _playerOneFigures, _playerTwoFigures })
	{
		for (auto figure : figures)
			_window.draw(figure->GetSprite());
	}

}

void ClearHighlitghts()
{
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{
			_tiles[i][j].Highlight(false);
		}
	}
}

void GameManager::Update() 
{
	std::optional<sf::Event> event;
	sf::Vector2i mousePos;
	Tile* selectedTile = &_tiles[0][0];
    Tile* previousSelectedTile = &_tiles[0][0];
	Figure* selectedFigure = nullptr;
    Figure* previousSelectedFigure = nullptr;

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
			if (event->is<sf::Event::MouseButtonPressed>() &&
				(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
			{
				mousePos = sf::Mouse::getPosition(_window);
                int tileX = mousePos.x / 128;
                int tileY = mousePos.y / 128;
                //cout << "Mouse clicked at tile X: " << tileX << " Y: " << tileY << endl;
				if (tileX >= 0 && tileX < 8 && tileY >= 0 && tileY < 8)
				{
					selectedTile = &_tiles[tileX][tileY];
                    cout << "Is tile occupied?" << (selectedTile->IsOccupied() ? " Yes" : " No") << endl;

					if (selectedTile->IsOccupied())
					{
						// Clear previous highlights
                        ClearHighlitghts();
						selectedFigure = selectedTile->GetFigure();
						//cout << "Fig pos: X: " << selectedFigure->GetX() << " Y: " << selectedFigure->GetY() << endl;
						
                        if (selectedFigure->GetColor() == _currentRound) // if the figure color matches the current player's turn
						{
							//cout << "Player with white figs clicked on a: " << typeid(*selectedFigure).name()
							//	<< " at the position X: " << selectedFigure->GetX() << " Y: " << selectedFigure->GetY() << endl;
							//cout << "Selected " << (selectedFigure->GetColor() ? "White " : "Black ") << typeid(*selectedFigure).name() << endl;
                            
                            // Show possible moves
							vector<Tile*> possibleMoves = selectedFigure->GetPossibleMoves(_tiles);
                            selectedFigure->HighlightPossibleMoves(possibleMoves);
                            // Wait for the player to select a valid move
							previousSelectedTile = selectedTile;
                            previousSelectedFigure = selectedFigure;

						}
						else
						{
                            cout << "It's not your turn!" << endl;
						}
					}
					else if (selectedTile->IsHighlighted())
					{
                        selectedFigure->Move(selectedTile);
                        // Clear highlights after move
                        ClearHighlitghts();
                        previousSelectedTile->SetFigure(nullptr); // Remove figure from previous tile
                        _currentRound = !_currentRound; // Switch turns
					}

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