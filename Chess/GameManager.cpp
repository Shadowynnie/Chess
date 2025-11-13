#include "GameManager.h"
#include "Colors.h"

#include "Figurines/Figure.h"
#include "Figurines/Bishop.h"
#include "Figurines/King.h"
#include "Figurines/Knight.h"
#include "Figurines/Rook.h"
#include "Figurines/Queen.h"
#include "Figurines/Pawn.h"

using std::cerr;

using std::cout;

/* TODO: 
* Handle checkmate and stalemate conditions and add game over screen with optoins to restart or go to main menu
* Implement networking for multiplayer mode
* Add paused menu with options to resume, restart(singleplayer mode), leave match(multiplayer) / go to main menu
* Add configuration menu to set network settings, sound settings, etc.
* Add AI player bot for singleplayer mode
* Get rid of global variables where possible
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
			_tiles[i][j] = Tile(i, j, false);
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
    cout << "Board initialized with pieces.\n";
}

void GameManager::DeinitializeBoard()
{
	// Clean up dynamically allocated memory for figures
	for (auto& figures : { _playerOneFigures, _playerTwoFigures })
	{
		for (auto figure : figures)
			delete figure;
	}
	cout << "Board deinitialized and memory cleaned up.\n";
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
				cout << (isWhiteKing ? "White" : "Black") << " King is in check!\n";
			}
			else
				kingTile->SetInCheck(false);
		}
	};
	checkKing(true);  // Check white king
    checkKing(false); // Check black king
}

void GameManager::PromotePawn(Figure* pawn)
{
	// Remove pawn from the game first
	int x = pawn->GetX();
	int y = pawn->GetY();
	bool isWhite = pawn->GetColor();

	auto& playerFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
	auto it = std::find(playerFigures.begin(), playerFigures.end(), pawn);
	if (it != playerFigures.end())
	{
		playerFigures.erase(it);
		delete pawn;
	}
	_tiles[x][y].SetFigure(nullptr); // clear tile occupant while choosing

	// Prepare promotion menu visuals
	const float itemSize = 128.f;
	const float padding = 10.f;
	const float menuWidth = itemSize + padding * 2.f;
	const float menuHeight = itemSize * 4.f + padding * 5.f;

	sf::Vector2u winSize = _window.getSize();
	float menuX = (float(winSize.x) - menuWidth) / 2.f;
	float menuY = (float(winSize.y) - menuHeight) / 2.f;

	sf::RectangleShape promoBackg(sf::Vector2f(menuWidth, menuHeight));
	promoBackg.setPosition(sf::Vector2f(menuX, menuY));
	promoBackg.setFillColor(sf::Color(30, 30, 30, 220));
	promoBackg.setOutlineColor(sf::Color::White);
	promoBackg.setOutlineThickness(3.f);

	// Load sprites for the 4 promotion choices (color specific)
	std::string suffix = isWhite ? "white" : "black";
	sf::Sprite rookSprite = AssetManager::GetSprite("rook_" + suffix);
	sf::Sprite knightSprite = AssetManager::GetSprite("knight_" + suffix);
	sf::Sprite bishopSprite = AssetManager::GetSprite("bishop_" + suffix);
	sf::Sprite queenSprite = AssetManager::GetSprite("queen_" + suffix);

	// Make sprites a bit larger for the menu (AssetManager already scales to 0.5),
	// adjust scale to visually fit the itemSize while keeping aspect ratio.
	auto fitSprite = [&](sf::Sprite& s, float targetSize)
	{
		sf::FloatRect bounds = s.getLocalBounds();
		float maxSide = std::max(bounds.size.x, bounds.size.y);
		if (maxSide <= 0.f) return;
		float scale = (targetSize - 24.f) / maxSide; // small margin
		s.setScale(sf::Vector2f(scale, scale));
	};

	fitSprite(rookSprite, itemSize);
	fitSprite(knightSprite, itemSize);
	fitSprite(bishopSprite, itemSize);
	fitSprite(queenSprite, itemSize);

	// Position sprites vertically inside the menu with padding
	std::array<sf::Sprite*, 4> items = { &rookSprite, &knightSprite, &bishopSprite, &queenSprite };
	for (size_t i = 0; i < items.size(); ++i)
	{
		float itemX = menuX + (menuWidth - items[i]->getGlobalBounds().size.x) / 2.f;
		float itemY = menuY + padding + i * (itemSize + padding) + (itemSize - items[i]->getGlobalBounds().size.y) / 2.f;
		items[i]->setPosition(sf::Vector2f(itemX, itemY));
	}

	// Map each sprite to its promotion choice (eliminates if/else chain)
	enum class PromotionChoice { ROOK, KNIGHT, BISHOP, QUEEN };
	std::map<sf::Sprite*, PromotionChoice> itemChoices
	{
		{&rookSprite, PromotionChoice::ROOK},
		{&knightSprite, PromotionChoice::KNIGHT},
		{&bishopSprite, PromotionChoice::BISHOP},
		{&queenSprite, PromotionChoice::QUEEN}
	};

	// Modal loop: block until player selects a promotion or window closed
	bool chosen = false;
	PromotionChoice choice = PromotionChoice::QUEEN; // default

	while (_window.isOpen() && !chosen)
	{
		std::optional<sf::Event> event;
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				DeinitializeBoard();
				_window.close();
				return;
			}
			if (event->is<sf::Event::MouseButtonPressed>() && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				// Use the mapping to detect which sprite was clicked
				for (auto& pair : itemChoices)
				{
					if (pair.first->getGlobalBounds().contains(mfp))
					{
						choice = pair.second;
						chosen = true;
						break;
					}
				}
			}
			// allow cancel/back to default queen on Esc
			if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				choice = PromotionChoice::QUEEN;
				chosen = true;
			}
		}

		// Draw current board underneath and overlay the modal
		_window.clear();
		DrawGame();
		_window.draw(promoBackg);
		// draw each item with a thin border highlight
		for (size_t i = 0; i < items.size(); ++i)
		{
			// item background box
			sf::RectangleShape itemBg(sf::Vector2f(itemSize, itemSize));
			float bx = menuX + padding;
			float by = menuY + padding + i * (itemSize + padding);
			itemBg.setPosition(sf::Vector2f(bx, by));
			itemBg.setFillColor(sf::Color(200, 200, 200, 20));
			itemBg.setOutlineColor(sf::Color(150, 150, 150, 200));
			itemBg.setOutlineThickness(1.f);
			_window.draw(itemBg);

			_window.draw(*items[i]);
		}
		_window.display();
	}

	// Map for creating the chosen piece (replaces switch)
	std::map<PromotionChoice, std::function<Figure* ()>> constructors;
	constructors[PromotionChoice::ROOK] = [x, y, isWhite]() { return new Rook(x, y, isWhite); };
	constructors[PromotionChoice::KNIGHT] = [x, y, isWhite]() { return new Knight(x, y, isWhite); };
	constructors[PromotionChoice::BISHOP] = [x, y, isWhite]() { return new Bishop(x, y, isWhite); };
	constructors[PromotionChoice::QUEEN] = [x, y, isWhite]() { return new Queen(x, y, isWhite); };

	Figure* newPiece = nullptr;
	auto itCtor = constructors.find(choice);
	if (itCtor != constructors.end())
		newPiece = itCtor->second();
	else
		newPiece = constructors[PromotionChoice::QUEEN](); // fallback

	if (newPiece)
	{
		playerFigures.push_back(newPiece);
		_tiles[x][y].SetFigure(newPiece);
		cout << (isWhite ? "White" : "Black") << " Pawn promoted to "
			<< (typeid(*newPiece) == typeid(Rook) ? "Rook" :
				typeid(*newPiece) == typeid(Knight) ? "Knight" :
				typeid(*newPiece) == typeid(Bishop) ? "Bishop" : "Queen")
			<< " at (" << x << ", " << y << ")\n";
	}
}

void GameManager::ResetEnPassantFlags()
{
	// Reset en passant flags for opponent pawns
	auto& allPawns = _currentRound ? _playerOneFigures : _playerTwoFigures;
	for (auto* fig : allPawns)
	{
		if (typeid(*fig) == typeid(Pawn))
			dynamic_cast<Pawn*>(fig)->EnableEnPassant(false);
	}
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
				return;
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

				cout << "Mouse clicked at tile X: " << tileX << " Y: " << tileY << "\n";

				if (tileX >= 0 && tileX < 8 && tileY >= 0 && tileY < 8)
				{
					selectedTile = &_tiles[tileX][tileY];

                    // Debug information
                    cout << "-------------------------\n";
                    cout << "Selected Tile - X: " << selectedTile->GetX() << " Y: " << selectedTile->GetY() << "\n";
                    cout << "Is tile occupied?" << (selectedTile->IsOccupied() ? " Yes" : " No") << "\n";
					//cout << "Is previous tile nullptr?" << (previousSelectedTile == nullptr ? " Yes\n" : " No\n");
					//cout << "Is tile highlighted?" << (selectedTile->IsHighlighted() ? " Yes\n" : " No\n");
                    cout << "Current round (true=white, false=black): " << (_currentRound ? "White\n" : "Black\n");
                    //cout << "Tile's figure pointer? " << (selectedTile->GetFigure() != nullptr ? " Yes\n" : " No\n");
					//cout << "Figure color on selected tile: " << (selectedTile->IsOccupied() ? (selectedTile->GetFigure()->GetColor() ? "White\n" : "Black\n") : "N/A\n");
                    //cout << "Figure position on selected tile: " << (selectedTile->IsOccupied() ? ("X: " + std::to_string(selectedTile->GetFigure()->GetX()) + " Y: " + std::to_string(selectedTile->GetFigure()->GetY())) : "N/A") << "\n";
                    //cout << "Is tile red? " << (selectedTile->IsInCheck() ? " Yes\n" : " No\n");
                    //cout << "Is it a Pawn or Queen? " << (selectedTile->IsOccupied() ? (typeid(*selectedTile->GetFigure()) == typeid(Pawn) ? " Pawn" : (typeid(*selectedTile->GetFigure()) == typeid(Queen) ? " Queen" : " No")) : " N/A") << "\n";
                    //cout << "Is it a Rook? " << (selectedTile->IsOccupied() ? (typeid(*selectedTile->GetFigure()) == typeid(Rook) ? " Yes" : " No") : " N/A") << " And did it move? " << (selectedTile->IsOccupied() ? (typeid(*selectedTile->GetFigure()) == typeid(Rook) ? (dynamic_cast<Rook*>(selectedTile->GetFigure())->HasMoved() ? " Yes" : " No") : " N/A") : " N/A") << "\n"

                    // When the player clicks on a highlighted tile to move
					if (previousSelectedTile != nullptr)
					{
                        // Move the figure to the selected tile
						if ((selectedTile->IsHighlighted()) && (previousSelectedTile->GetFigure()->GetColor() == _currentRound))
						{
							if (selectedTile->IsOccupied())
							{	// Capture opponent's piece
								if (selectedTile->GetFigure()->GetColor() != _currentRound)
									previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, selectedTile->GetFigure()->GetColor() != true ? _playerTwoFigures : _playerOneFigures);
							}
							else // Normal move to empty tile
							{	//previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile,_tiles);
								if (typeid(*previousSelectedTile->GetFigure()) == typeid(Pawn))
								{
									Pawn* pawn = dynamic_cast<Pawn*>(previousSelectedTile->GetFigure());
									if (pawn)
										pawn->Move(selectedTile, previousSelectedTile, _currentRound ? _playerTwoFigures : _playerOneFigures, _tiles);
								}
								else
									previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, _tiles);
							}
							
                            // Check for pawn promotion
                            if (typeid(*selectedTile->GetFigure()) == typeid(Pawn))
							{
								Pawn* pawn = dynamic_cast<Pawn*>(selectedTile->GetFigure());
								if (pawn && pawn->CanPromote())
									PromotePawn(pawn);
                            }

							ClearHighlitghts();
							// Switch turns
							_currentRound = !_currentRound;
							previousSelectedTile = nullptr;
							// Reset en passant flags for opponent pawns
							ResetEnPassantFlags();
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
					cout << "Single player -> player vs computer.\n";
					// Transition to singleplayer game state
                    _currentState = GameState::SINGLEPLAYER;
				}
				if (hostButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Host Game button clicked!\n";
					// Transition to host game state
                    _currentState = GameState::HOST_GAME;
				}
				else if (joinButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Join Game button clicked!\n";
					// Transition to join game state
                    _currentState = GameState::CONNECT_TO_GAME;
				}
				else if (quitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
				{
					cout << "Quit button clicked!\n";
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
	cout << "Hosting a Game...\n";
	// Placeholder for hosting game logic
}

void GameManager::ConnectToGame()
{
	cout << "Connecting to a Game...\n";
	// Placeholder for connecting to game logic
}

void GameManager::PlayGame()
{
	cout << "Starting the Game...\n";
	// Placeholder for game loop logic
	InitializeBoard();
    Update();
}