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
* Implement networking for multiplayer mode
* Add configuration menu to set network settings, sound settings, etc.
* Add AI player bot for singleplayer mode
* Get rid of global variables where possible
*/

NetworkManager _networkMgr;
std::thread _networkThread;
static bool _isHosting = false; // true when this instance is the server/host
static std::mutex s_gameMutex; // to protect game state for safe cross-thread access

static bool _localIsWhite = true; // true = this instance plays White, false = Black
static bool _awaitingServer = false; // when client has sent move and waits for authoritative broadcast

static bool _currentRound = true; // True for player 1's turn, false for player 2's turn
GameState _currentState;
GameResult _gameResult;
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
	{ GameState::SINGLEPLAYER, []() { GameManager::PlayGame(false); }},
	{ GameState::GAME_OVER, []() { GameManager::EndGameMenu(); }}, // Start GameOver menu here
	{ GameState::CLOSED, [&]() { _window.close(); }},
	{ GameState::SETTINGS, [&]() { GameManager::SettingsMenu(); }}
};

void GameManager::ShutdownNetwork()
{
	// Stop the network service loop (thread will exit its loop)
	_networkMgr.StopServiceLoop();

	// Join the network thread if it was started
	if (_networkThread.joinable())
		_networkThread.join();
}

// Returns whether the given side's king is currently in check.
bool GameManager::IsKingInCheck(bool isWhite)
{
	auto& friendlyFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
	auto& enemyFigures = isWhite ? _playerTwoFigures : _playerOneFigures;

	King* king = nullptr;
	for (auto fig : friendlyFigures)
	{
		if (typeid(*fig) == typeid(King))
		{
			king = dynamic_cast<King*>(fig);
			break;
		}
	}
	if (!king)
		return false; // no king found (shouldn't happen in normal play)

	return king->IsThreatened(_tiles, enemyFigures);
}

// Return true if the given side (isWhite) has at least one legal move.
// A legal move is any move the side can make that does not leave its own king in check.
// We simulate each candidate move, temporarily updating tiles and piece positions, and test king safety.
// This intentionally keeps changes local (we revert them after the test).
bool GameManager::HasAnyLegalMoves(bool isWhite)
{
	auto& friendlyFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
	auto& enemyFigures = isWhite ? _playerTwoFigures : _playerOneFigures;

	for (auto fig : friendlyFigures)
	{
		// Obtain candidate moves.
		vector<Tile*> candidateMoves;
		if (typeid(*fig) == typeid(King))
			candidateMoves = fig->GetPossibleMoves(_tiles, enemyFigures); // king-aware moves
		else
			candidateMoves = fig->GetPossibleMoves(_tiles);

		if (candidateMoves.empty())
			continue;

		// For each candidate, simulate and test whether king is safe after the move
		for (auto targetTile : candidateMoves)
		{
			Tile* sourceTile = fig->GetCurrentTile(_tiles);
			if (!sourceTile) continue;

			Figure* captured = targetTile->GetFigure();
			int oldX = fig->GetX();
			int oldY = fig->GetY();

			// Apply the move on the board (temporary)
			sourceTile->SetFigure(nullptr);
			targetTile->SetFigure(fig);
			fig->setPosition(targetTile->GetX(), targetTile->GetY());

			// Build local enemy list that reflects captured piece removal (if any)
			vector<Figure*> localEnemies = enemyFigures;
			if (captured)
			{
				auto it = std::find(localEnemies.begin(), localEnemies.end(), captured);
				if (it != localEnemies.end())
					localEnemies.erase(it);
			}

			// Find this side's king (might be f itself if king moved)
			King* king = nullptr;
			if (typeid(*fig) == typeid(King))
				king = dynamic_cast<King*>(fig);
			else
			{
				for (auto friendFig : friendlyFigures)
				{
					if (typeid(*friendFig) == typeid(King))
					{
						king = dynamic_cast<King*>(friendFig);
						break;
					}
				}
			}

			bool kingInCheck = false;
			if (king)
				kingInCheck = king->IsThreatened(_tiles, localEnemies);

			// Revert the move
			sourceTile->SetFigure(fig);
			targetTile->SetFigure(captured);
			fig->setPosition(oldX, oldY);

			// If king is safe after this move => we have at least one legal move
			if (!kingInCheck)
				return true;
		}
	}
	// No legal moves found
	return false;
}

// Return true when both sides have only their kings remaining (insufficient material)
bool GameManager::OnlyKingsLeft()
{
	auto countNonKings = [](const vector<Figure*>& figures)
	{
		int count = 0;
		for (auto fig : figures)
		{
			if (typeid(*fig) != typeid(King))
				++count;
		}
		return count;
	};

	return (countNonKings(_playerOneFigures) == 0) && (countNonKings(_playerTwoFigures) == 0);
}

// Evaluate endgame after a move. Prints result to console.
// Returns true if the game ended (caller may choose to stop the game loop).
bool GameManager::EvaluateEndGame()
{
	// Insufficient material (only kings)
	if (OnlyKingsLeft())
	{
		cout << "Game ended: Draw (Only kings remain).\n";
		_currentState = GameState::GAME_OVER;
		stateFunctions[_currentState]();
		return true;
	}

	// Side to move
	bool toMoveIsWhite = _currentRound;

	// Check whether side to move is in check
	bool inCheck = IsKingInCheck(toMoveIsWhite);
	bool hasMoves = HasAnyLegalMoves(toMoveIsWhite);

	if (!hasMoves)
	{
		if (inCheck)
		{
			// Checkmate -> the side who just moved wins (opposite of toMove)
			cout << "Checkmate! " << (toMoveIsWhite ? "Black" : "White") << " wins.\n";
			_gameResult = (toMoveIsWhite ? GameResult::BLACK_WINS : GameResult::WHITE_WINS);
			_currentState = GameState::GAME_OVER;
			stateFunctions[_currentState]();
			return true;
		}
		else
		{
			// Stalemate -> draw
			cout << "Stalemate! Draw.\n";
			_gameResult = GameResult::DRAW;
			_currentState = GameState::GAME_OVER;
			stateFunctions[_currentState]();
			return true;
		}
	}

	// No game end
	return false;
}

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
	map<PieceType, function<Figure* (int, int, bool)>> factories = 
	{
		{ PieceType::ROOK,   [](int x,int y,bool w) { return new Rook(x,y,w); } },
		{ PieceType::KNIGHT, [](int x,int y,bool w) { return new Knight(x,y,w); } },
		{ PieceType::BISHOP, [](int x,int y,bool w) { return new Bishop(x,y,w); } },
		{ PieceType::QUEEN,  [](int x,int y,bool w) { return new Queen(x,y,w); } },
		{ PieceType::KING,   [](int x,int y,bool w) { return new King(x,y,w); } },
		{ PieceType::PAWN,   [](int x,int y,bool w) { return new Pawn(x,y,w); } },
	};

	// Back rank layout
	std::array<PieceType, 8> backRank = 
	{
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
	const vector<Figure*>& white,
	const vector<Figure*>& black)
{
	// Draw each occupied tile's figure once (authoritative)
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			Tile& t = _tiles[x][y];
			if (t.IsOccupied())
			{
				Figure* f = t.GetFigure();
				if (f)
					window.draw(f->GetSprite());
			}
		}
	}
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
	int x = pawn->GetX();
	int y = pawn->GetY();
	bool isWhite = pawn->GetColor();

	// choose promotion (modal)
	uint8_t promoCode = SelectPromotionChoice(x, y, isWhite); // 1=Q,2=R,3=B,4=N

	// remove pawn from player's list
	auto& playerFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
	auto it = std::find(playerFigures.begin(), playerFigures.end(), pawn);
	if (it != playerFigures.end())
	{
		playerFigures.erase(it);
		delete pawn;
	}

	// create chosen piece and place it
	Figure* newPiece = CreatePromotionPiece(x, y, isWhite, promoCode);
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

uint8_t GameManager::SelectPromotionChoice(int x, int y, bool isWhite)
{
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

	string suffix = isWhite ? "white" : "black";
	sf::Sprite rookSprite = AssetManager::GetSprite("rook_" + suffix);
	sf::Sprite knightSprite = AssetManager::GetSprite("knight_" + suffix);
	sf::Sprite bishopSprite = AssetManager::GetSprite("bishop_" + suffix);
	sf::Sprite queenSprite = AssetManager::GetSprite("queen_" + suffix);

	auto fitSprite = [&](sf::Sprite& s, float targetSize)
		{
			sf::FloatRect bounds = s.getLocalBounds();
			float maxSide = std::max(bounds.size.x, bounds.size.y);
			if (maxSide <= 0.f) return;
			float scale = (targetSize - 24.f) / maxSide;
			s.setScale(sf::Vector2f(scale, scale));
		};
	fitSprite(rookSprite, itemSize);
	fitSprite(knightSprite, itemSize);
	fitSprite(bishopSprite, itemSize);
	fitSprite(queenSprite, itemSize);

	std::array<std::pair<sf::Sprite*, uint8_t>, 4> items =
	{
		std::make_pair(&queenSprite, uint8_t(1)), // prefer queen on top
		std::make_pair(&rookSprite, uint8_t(2)),
		std::make_pair(&bishopSprite, uint8_t(3)),
		std::make_pair(&knightSprite, uint8_t(4))
	};

	for (size_t i = 0; i < items.size(); ++i)
	{
		float itemX = menuX + (menuWidth - items[i].first->getGlobalBounds().size.x) / 2.f;
		float itemY = menuY + padding + i * (itemSize + padding) + (itemSize - items[i].first->getGlobalBounds().size.y) / 2.f;
		items[i].first->setPosition(sf::Vector2f(itemX, itemY));
	}

	uint8_t chosen = 1; // default Queen
	bool done = false;
	while (_window.isOpen() && !done)
	{
		std::optional<sf::Event> event;
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				DeinitializeBoard();
				_window.close();
				return chosen;
			}
			if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				for (auto& p : items)
				{
					if (p.first->getGlobalBounds().contains(mfp))
					{
						chosen = p.second;
						done = true;
						break;
					}
				}
			}
			if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				chosen = 1;
				done = true;
			}
		}

		_window.clear();
		DrawGame();
		_window.draw(promoBackg);
		for (size_t i = 0; i < items.size(); ++i)
		{
			// draw background box
			sf::RectangleShape itemBg(sf::Vector2f(itemSize, itemSize));
			float bx = menuX + padding;
			float by = menuY + padding + i * (itemSize + padding);
			itemBg.setPosition(sf::Vector2f(bx, by));
			itemBg.setFillColor(sf::Color(200, 200, 200, 20));
			itemBg.setOutlineColor(sf::Color(150, 150, 150, 200));
			itemBg.setOutlineThickness(1.f);
			_window.draw(itemBg);
			_window.draw(*items[i].first);
		}
		_window.display();
	}
	return chosen;
}

// Create promotion Figure based on code (1=Q,2=R,3=B,4=N)
Figure* GameManager::CreatePromotionPiece(int x, int y, bool isWhite, uint8_t promotionCode)
{
	switch (promotionCode)
	{
	case 2: return new Rook(x, y, isWhite);
	case 3: return new Bishop(x, y, isWhite);
	case 4: return new Knight(x, y, isWhite);
	case 1:
	default:
		return new Queen(x, y, isWhite);
	}
}

/*
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
	string suffix = isWhite ? "white" : "black";
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
	map<sf::Sprite*, PromotionChoice> itemChoices
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
	map<PromotionChoice, function<Figure* ()>> constructors;
	constructors[PromotionChoice::ROOK] = [x, y, isWhite]() { return new Rook(x, y, isWhite); };
	constructors[PromotionChoice::KNIGHT] = [x, y, isWhite]() { return new Knight(x, y, isWhite); };
	constructors[PromotionChoice::BISHOP] = [x, y, isWhite]() { return new Bishop(x, y, isWhite); };
	constructors[PromotionChoice::QUEEN] = [x, y, isWhite]() { return new Queen(x, y, isWhite); };

	Figure* newPiece = nullptr;
	//auto itCtor = constructors[choice];
	if (choice != PromotionChoice::QUEEN)
		newPiece = constructors[choice]();
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
}*/

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

//====================================TESTING========================================
void sendTestData(NetworkManager& nwmgr)
{
	string testData = "Sample FEN string data for testing.";
	cout << "Sending test data...\n";
	nwmgr.SendTextReliable(nwmgr.GetPeer(), testData, MessageType::FULL_FEN);
}
//===================================================================================

std::mutex& GameManager::GetGameMutex()
{
	return s_gameMutex;
}

// Apply a move without validating (used by clients when server broadcasts authoritative move).
void GameManager::ApplyMoveLocal(const MoveMessage& m)
{
	// Expect caller holds mutex or is main thread; we'll lock to be safe
	std::lock_guard lock(s_gameMutex);

	if (m.fromX > 7 || m.fromY > 7 || m.toX > 7 || m.toY > 7)
		return;

	Tile* source = &_tiles[m.fromX][m.fromY];
	Tile* target = &_tiles[m.toX][m.toY];
	if (!source || !target)
		return;

	Figure* fig = source->GetFigure();
	if (!fig)
		return;

	// If target occupied, remove captured piece from appropriate vector
	Figure* captured = target->GetFigure();
	if (captured)
	{
		auto& victimList = captured->GetColor() ? _playerOneFigures : _playerTwoFigures;
		auto it = std::find(victimList.begin(), victimList.end(), captured);
		if (it != victimList.end())
		{
			victimList.erase(it);
			delete captured;
		}
	}

	// Move the figure (use existing Move overloads)
	// Attempt pawn-aware overload if this is a Pawn
	bool movedPawn = false;
	if (typeid(*fig) == typeid(Pawn))
	{
		Pawn* pawn = dynamic_cast<Pawn*>(fig);
		if (pawn)
		{
			pawn->Move(target, source, _currentRound ? _playerTwoFigures : _playerOneFigures, _tiles);
			movedPawn = true;
		}
	}
	else
	{
		fig->Move(target, source, _tiles);
	}

	// Promotion: if the moved piece is a Pawn and is promotable, handle according to message
	if (movedPawn)
	{
		Figure* occupant = target->GetFigure();
		if (occupant && typeid(*occupant) == typeid(Pawn))
		{
			Pawn* pawn = dynamic_cast<Pawn*>(occupant);
			if (pawn && pawn->CanPromote())
			{
				// If promotion code provided by network -> apply it silently
				if (m.promotion >= 1 && m.promotion <= 4)
				{
					int x = target->GetX();
					int y = target->GetY();
					bool isWhite = pawn->GetColor();
					// remove pawn from player's list
					auto& playerFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
					auto it = std::find(playerFigures.begin(), playerFigures.end(), pawn);
					if (it != playerFigures.end())
					{
						playerFigures.erase(it);
						delete pawn;
					}
					_tiles[x][y].SetFigure(nullptr);
					Figure* newPiece = CreatePromotionPiece(x, y, isWhite, m.promotion);
					if (newPiece)
					{
						playerFigures.push_back(newPiece);
						_tiles[x][y].SetFigure(newPiece);
					}
				}
				else
				{
					// fallback: interactive UI on this client
					PromotePawn(pawn);
				}
			}
		}
	}

	// Update turn and flags
	ClearHighlitghts();
	_currentRound = !_currentRound;
	ResetEnPassantFlags();

	// Clear awaiting state (in case a client was waiting for server confirmation)
	_awaitingServer = false;

	// Evaluate endgame
	EvaluateEndGame();
}

// Server-side: validate move against authoritative board, apply it if legal, return true if applied.
// IMPORTANT: Do NOT apply the move here (network thread). Validation only.
// The main (UI) thread will apply the authoritative move via ApplyMoveLocal().
bool GameManager::ServerValidateAndApplyMove(const MoveMessage& m)
{
	std::lock_guard lock(s_gameMutex);

	// basic bounds
	if (m.fromX > 7 || m.fromY > 7 || m.toX > 7 || m.toY > 7)
		return false;

	Tile* source = &_tiles[m.fromX][m.fromY];
	Tile* target = &_tiles[m.toX][m.toY];
	if (!source || !target) return false;

	Figure* fig = source->GetFigure();
	if (!fig) return false;

	// must be side to move
	if (fig->GetColor() != _currentRound) return false;

	// obtain candidate moves (king-aware)
	vector<Tile*> possibleMoves;
	if (typeid(*fig) == typeid(King))
	{
		auto& enemyFigures = _currentRound ? _playerTwoFigures : _playerOneFigures;
		possibleMoves = fig->GetPossibleMoves(_tiles, enemyFigures);
	}
	else
		possibleMoves = fig->GetPossibleMoves(_tiles);

	// check if target is among possibleMoves
	auto itMove = std::find(possibleMoves.begin(), possibleMoves.end(), target);
	if (itMove == possibleMoves.end())
		return false;

	// simulate move and test whether king is safe after the move
	Figure* captured = target->GetFigure();
	int oldX = fig->GetX();
	int oldY = fig->GetY();

	// Apply the move on the board (temporary)
	source->SetFigure(nullptr);
	target->SetFigure(fig);
	fig->setPosition(target->GetX(), target->GetY()); // NOTE: this only updates logical X/Y (no sprite)

	// Build local enemy list that reflects captured piece removal (if any)
	auto& enemyFigures = _currentRound ? _playerTwoFigures : _playerOneFigures;
	vector<Figure*> localEnemies = enemyFigures;
	if (captured)
	{
		auto rit = std::find(localEnemies.begin(), localEnemies.end(), captured);
		if (rit != localEnemies.end())
			localEnemies.erase(rit);
	}

	// Find this side's king (might be the moved piece)
	King* king = nullptr;
	if (typeid(*fig) == typeid(King))
		king = dynamic_cast<King*>(fig);
	else
	{
		auto& friendly = _currentRound ? _playerOneFigures : _playerTwoFigures;
		for (auto friendFig : friendly)
		{
			if (typeid(*friendFig) == typeid(King))
			{
				king = dynamic_cast<King*>(friendFig);
				break;
			}
		}
	}

	bool kingInCheck = false;
	if (king)
		kingInCheck = king->IsThreatened(_tiles, localEnemies);

	// revert temporary simulation (we do not apply here)
	source->SetFigure(fig);
	target->SetFigure(captured);
	fig->setPosition(oldX, oldY);

	if (kingInCheck)
	{
		return false;
	}

	if (typeid(*fig) == typeid(Pawn))
	{
		bool isWhitePawn = fig->GetColor();
		if ((isWhitePawn && m.toY == 7) || (!isWhitePawn && m.toY == 0))
		{
			if (m.promotion < 1 || m.promotion > 4)
				return false; // invalid promotion request
		}
	}

	// Move is legal. DO NOT apply it here on the network thread.
	// Return true so the caller (network thread) can broadcast and enqueue the move
	// for the main thread to apply (ApplyMoveLocal).
	return true;
}


void GameManager::Update(bool isMultiplayer)
{
	std::optional<sf::Event> event;
	sf::Vector2i mousePos;
	Tile* selectedTile = nullptr;
	Tile* previousSelectedTile = nullptr;

	// GAME LOOP
	while (_window.isOpen())
	{
		// First: consume any incoming authoritative moves queued by NetworkManager
		MoveMessage incoming;
		while (_networkMgr.TryPopIncomingMove(incoming))
		{
			// apply on main thread (no race with network thread)
			ApplyMoveLocal(incoming);
		}

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
			if (event->is<sf::Event::KeyPressed>() &&
				(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)))
			{
                PausedMenu();
			}
			if (event->is<sf::Event::MouseButtonPressed>() &&
				(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)))
			{
				mousePos = sf::Mouse::getPosition(_window);
				int tileX = mousePos.x / 128;
				int tileY = mousePos.y / 128;

				//cout << "Mouse clicked at tile X: " << tileX << " Y: " << tileY << "\n";

				if (tileX >= 0 && tileX < 8 && tileY >= 0 && tileY < 8)
				{
					selectedTile = &_tiles[tileX][tileY];

					// Debug information
					cout << "-------------------------\n";
					cout << "Selected Tile - X: " << selectedTile->GetX() << " Y: " << selectedTile->GetY() << "\n";
					cout << "Is tile occupied?" << (selectedTile->IsOccupied() ? " Yes\n" : " No\n");
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
						// Enforce multiplayer restrictions: local player must be allowed to move
						if (isMultiplayer && (_awaitingServer || (_localIsWhite != _currentRound)))
						{
							// either waiting for server or not our side to move -> ignore click
							ClearHighlitghts();
							previousSelectedTile = nullptr;
						}
						else if ((selectedTile->IsHighlighted()) && (previousSelectedTile->GetFigure()->GetColor() == _currentRound))
						{
							// Build MoveMessage (simple payload)
							MoveMessage moveMsg;
							moveMsg.fromX = static_cast<uint8_t>(previousSelectedTile->GetX());
							moveMsg.fromY = static_cast<uint8_t>(previousSelectedTile->GetY());
							moveMsg.toX = static_cast<uint8_t>(selectedTile->GetX());
							moveMsg.toY = static_cast<uint8_t>(selectedTile->GetY());
							moveMsg.promotion = 0; // TODO: set promotion choice when implemented
							moveMsg.flags = 0;

							if (isMultiplayer)
							{
								if (_isHosting)
								{
									// Server: validate & apply locally (this path already existed)
									if (selectedTile->IsOccupied())
									{   // Capture opponent's piece
										if (selectedTile->GetFigure()->GetColor() != _currentRound)
											previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, selectedTile->GetFigure()->GetColor() != true ? _playerTwoFigures : _playerOneFigures);
									}
									else // Normal move to empty tile
									{
										if (typeid(*previousSelectedTile->GetFigure()) == typeid(Pawn))
										{
											Pawn* pawn = dynamic_cast<Pawn*>(previousSelectedTile->GetFigure());
											if (pawn)
												pawn->Move(selectedTile, previousSelectedTile, _currentRound ? _playerTwoFigures : _playerOneFigures, _tiles);
										}
										else
											previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, _tiles);
									}

									// Promotion
									if (typeid(*selectedTile->GetFigure()) == typeid(Pawn))
									{
										Pawn* pawn = dynamic_cast<Pawn*>(selectedTile->GetFigure());
										if (pawn && pawn->CanPromote())
										{
											int tx = selectedTile->GetX();
											int ty = selectedTile->GetY();
											bool isWhite = pawn->GetColor();
											uint8_t promo = SelectPromotionChoice(tx, ty, isWhite);
											moveMsg.promotion = promo;
											// replace pawn locally with chosen piece (server-authoritative)
											auto& playerFigures = isWhite ? _playerOneFigures : _playerTwoFigures;
											auto it = std::find(playerFigures.begin(), playerFigures.end(), pawn);
											if (it != playerFigures.end())
											{
												playerFigures.erase(it);
												delete pawn;
											}
											_tiles[tx][ty].SetFigure(nullptr);
											Figure* newPiece = CreatePromotionPiece(tx, ty, isWhite, promo);
											if (newPiece)
											{
												playerFigures.push_back(newPiece);
												_tiles[tx][ty].SetFigure(newPiece);
											}
										}
									}

									// Broadcast to peers (NetworkManager already does this on server validate; for server-initiated moves, send directly)
									ENetPeer* peer = _networkMgr.GetPeer();
									if (peer)
										NetworkManager::SendMoveReliable(peer, moveMsg);

									// finish turn
									ClearHighlitghts();
									_currentRound = !_currentRound;
									previousSelectedTile = nullptr;
									ResetEnPassantFlags();

									if (EvaluateEndGame())
									{
										_window.close();
										return;
									}
								}
								else
								{
									// Client: send move to server and wait for authoritative update
									ENetPeer* serverPeer = _networkMgr.GetPeer();
									if (serverPeer)
									{
										if (typeid(*previousSelectedTile->GetFigure()) == typeid(Pawn))
										{
											bool isWhitePawn = previousSelectedTile->GetFigure()->GetColor();
											int toY = selectedTile->GetY();
											bool willPromote = (isWhitePawn && toY == 7) || (!isWhitePawn && toY == 0);
											
											if (willPromote)
											{
												uint8_t promo = SelectPromotionChoice(static_cast<int>(selectedTile->GetX()), toY, isWhitePawn);
												moveMsg.promotion = promo;
											}
										}
										bool ok = NetworkManager::SendMoveReliable(serverPeer, moveMsg);
										if (!ok)
											cerr << "Failed to send move to server\n";
										// disable further input until server confirms/broadcasts
										_awaitingServer = true;
										ClearHighlitghts();
										previousSelectedTile = nullptr;
									}
									else
										cerr << "No server peer available to send move\n";
								}
							}
							else // singleplayer: apply move locally as before
							{
								if (selectedTile->IsOccupied())
								{   // Capture opponent's piece
									if (selectedTile->GetFigure()->GetColor() != _currentRound)
										previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, selectedTile->GetFigure()->GetColor() != true ? _playerTwoFigures : _playerOneFigures);
								}
								else // Normal move to empty tile
								{
									if (typeid(*previousSelectedTile->GetFigure()) == typeid(Pawn))
									{
										Pawn* pawn = dynamic_cast<Pawn*>(previousSelectedTile->GetFigure());
										if (pawn)
											pawn->Move(selectedTile, previousSelectedTile, _currentRound ? _playerTwoFigures : _playerOneFigures, _tiles);
									}
									else
										previousSelectedTile->GetFigure()->Move(selectedTile, previousSelectedTile, _tiles);
								}

								// Promotion
								if (typeid(*selectedTile->GetFigure()) == typeid(Pawn))
								{
									Pawn* pawn = dynamic_cast<Pawn*>(selectedTile->GetFigure());
									if (pawn && pawn->CanPromote())
										PromotePawn(pawn);
								}

								ClearHighlitghts();
								_currentRound = !_currentRound;
								previousSelectedTile = nullptr;
								ResetEnPassantFlags();

								if (EvaluateEndGame())
								{
									_window.close();
									return;
								}
							}
						}
					}
					// When the player clicks on a figure for the first time
					if (selectedTile->IsOccupied())
					{
						// The piece must belong to the side to move
						if (selectedTile->GetFigure()->GetColor() == _currentRound)
						{
							// In multiplayer, only allow interaction when this client controls the side to move
							if (isMultiplayer)
							{
								// Only allow selection when this client controls the side to move
								// and it's not waiting for server confirmation.
								if (!_awaitingServer && (_localIsWhite == _currentRound))
								{
									// Allowed: show possible moves for the selected piece
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
								// otherwise: ignore selection (no highlighting, no input)
							}
							else
							{
								// Singleplayer: same as before
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
						}
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

	// Layout parameters
	const float btnW = 400.f;
	const float btnH = 84.f;
	const float spacing = 24.f;
	const float paddingTop = 120.f;

	sf::Vector2u winSize = _window.getSize();
	float menuX = (float(winSize.x) - btnW) / 2.f;
	float menuY = paddingTop;

	// Load font
	sf::Font font;
	string fontPath = "Assets/Fonts/arial.ttf";
	if (!font.openFromFile(fontPath))
		cerr << "Failed to load font: " << fontPath << "\n";

	// Title
	sf::Text title(font);
    title.setString("CHESS");
    title.setCharacterSize(56u);
	title.setFillColor(sf::Color::White);
	{
		sf::FloatRect tb = title.getLocalBounds();
		
		title.setPosition(sf::Vector2f(
			(float(winSize.x) - tb.size.x) / 2.f - tb.position.x,
			40.f
		));
	}

	// Menu items text
	std::array<string, 5> labels =
	{	"SINGLEPLAYER",
		"HOST GAME",
		"JOIN GAME",
		"SETTINGS",
		"QUIT" 
	};

	// Create button shapes and texts
	vector<sf::RectangleShape> buttons;
	vector<sf::Text> texts;
	buttons.reserve(labels.size());
	texts.reserve(labels.size());

	for (size_t i = 0; i < labels.size(); ++i)
	{
		sf::RectangleShape rect(sf::Vector2f(btnW, btnH));
		rect.setPosition(sf::Vector2f(menuX, menuY + i * (btnH + spacing)));
		rect.setFillColor(sf::Color(40, 40, 40, 220));
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(2.f);

		sf::Text txt(font);
        txt.setString(labels[i]);
        txt.setCharacterSize(24u);
		txt.setFillColor(sf::Color::White);

		// center text inside rect
		sf::FloatRect tb = txt.getLocalBounds();
		sf::FloatRect rb = rect.getGlobalBounds();
		txt.setPosition(sf::Vector2f(
            rb.position.x + (rb.size.x - tb.size.x) / 2.f - tb.position.x,
			rb.position.y + (rb.size.y - tb.size.y) / 2.f - tb.position.y
		));

		buttons.push_back(rect);
		texts.push_back(txt);
	}

	std::optional<sf::Event> event;
	bool running = true;

	while (_window.isOpen() && running)
	{
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				_window.close();
				return;
			}
			if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				_currentState = GameState::CLOSED;
				stateFunctions[_currentState]();
				return;
			}
			if (event->is<sf::Event::MouseMoved>())
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				// hover effect
				for (size_t i = 0; i < buttons.size(); ++i)
				{
					if (buttons[i].getGlobalBounds().contains(mfp))
						buttons[i].setFillColor(sf::Color(80, 80, 80, 240));
					else
						buttons[i].setFillColor(sf::Color(40, 40, 40, 220));
				}
			}
			if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));

				for (size_t i = 0; i < buttons.size(); ++i)
				{
					if (buttons[i].getGlobalBounds().contains(mfp))
					{
                        if (i == 0) // SINGLEPLAYER
							_currentState = GameState::SINGLEPLAYER;
                        else if (i == 1) // HOST GAME
							_currentState = GameState::HOST_GAME;
                        else if (i == 2) // JOIN GAME
							_currentState = GameState::CONNECT_TO_GAME;
                        else if (i == 3) // SETTINGS
                            _currentState = GameState::SETTINGS;
						else if (i == 4) // QUIT
						{
							ShutdownNetwork();
							_currentState = GameState::CLOSED;
						}
						// Transition to chosen state and run its handler
						running = false;
						stateFunctions[_currentState]();
						break;
					}
				}
			}
		}

		_window.clear();
		// optional background: draw current board if any, otherwise just dark bg
		_window.clear(sf::Color(20, 20, 20));
		_window.draw(title);
		for (size_t i = 0; i < buttons.size(); ++i)
		{
			_window.draw(buttons[i]);
			_window.draw(texts[i]);
		}
		_window.display();
	}
}

void GameManager::EndGameMenu()
{
	// Frame / modal layout (similar style to PromotePawn)
	const float itemSize = 180.f;
	const float padding = 40.f;
	const float menuWidth = (itemSize * 2.f) + padding * 3.f; // two buttons + paddings
	const float menuHeight = itemSize + padding * 4.f;        // label + buttons area

	sf::Vector2u winSize = _window.getSize();
	float menuX = (float(winSize.x) - menuWidth) / 2.f;
	float menuY = (float(winSize.y) - menuHeight) / 2.f;

	sf::RectangleShape backg(sf::Vector2f(menuWidth, menuHeight));
	backg.setPosition(sf::Vector2f(menuX, menuY));
	backg.setFillColor(sf::Color(30, 30, 30, 230));
	backg.setOutlineColor(sf::Color::White);
	backg.setOutlineThickness(3.f);

	// Load Arial font (try common locations)
	sf::Font font;
	const string fontPath = "Assets/Fonts/arial.ttf";
	// Load the font or try alternative path relative to exe
	if (!font.openFromFile(fontPath))
		cerr << "Failed to load font from: " << fontPath << "\n";

	// Endgame label based on _gameResult
	string labelStr;

	if (_gameResult == GameResult::WHITE_WINS)
		labelStr = "White Wins!";
	else if (_gameResult == GameResult::BLACK_WINS)
		labelStr = "Black Wins!";
	else
		labelStr = "Draw";

	sf::Text label(font);
	label.setString(labelStr);
	label.setCharacterSize(34u);
	label.setFillColor(sf::Color::White);
	label.setStyle(sf::Text::Bold);

	// center label horizontally in frame
	sf::FloatRect lblBounds = label.getLocalBounds();
	label.setPosition(sf::Vector2f(
		menuX + (menuWidth - lblBounds.position.x) / 2.f - lblBounds.size.x,
		menuY + padding
	));

	// Button texts
	sf::Text backTxt(font);
	backTxt.setString("BACK TO \nMAIN MENU");
	backTxt.setCharacterSize(20u);
	backTxt.setFillColor(sf::Color::White);

	sf::Text retryTxt(font);
	retryTxt.setString("RETRY MATCH");
	retryTxt.setCharacterSize(20u);
	retryTxt.setFillColor(sf::Color::White);

	// Button background rectangles
	float btnW = itemSize;
	float btnH = itemSize * 0.5f;
	float btnY = menuY + padding + lblBounds.position.y + padding;

	sf::RectangleShape backBtn(sf::Vector2f(btnW, btnH));
	sf::RectangleShape retryBtn(sf::Vector2f(btnW, btnH));

	// Positions (two buttons next to each other)
	backBtn.setPosition(sf::Vector2f(menuX + padding, btnY));
	retryBtn.setPosition(sf::Vector2f(menuX + padding * 2.f + btnW, btnY));

	backBtn.setFillColor(sf::Color(70, 70, 70, 220));
	retryBtn.setFillColor(sf::Color(70, 70, 70, 220));
	backBtn.setOutlineColor(sf::Color::White);
	retryBtn.setOutlineColor(sf::Color::White);
	backBtn.setOutlineThickness(1.f);
	retryBtn.setOutlineThickness(1.f);

	// Center texts in buttons
	auto centerTextInRect = [&](sf::Text& t, const sf::RectangleShape& r) 
	{
		sf::FloatRect tb = t.getLocalBounds();
		sf::FloatRect rb = r.getGlobalBounds();
		// use position/size fields
		t.setPosition(sf::Vector2f(
			rb.position.x + (rb.size.x - tb.size.x) / 2.f - tb.position.x,
			rb.position.y + (rb.size.y - tb.size.y) / 2.f - tb.position.y
		));
	};

	centerTextInRect(backTxt, backBtn);
	centerTextInRect(retryTxt, retryBtn);

	std::optional<sf::Event> event;
	bool chosen = false;

	// Game over menu loop
	while (_window.isOpen() && !chosen)
	{
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				DeinitializeBoard();
				_window.close();
				return;
			}
			if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				// Back to main menu
				if (backBtn.getGlobalBounds().contains(mfp))
				{
					// cleanup and go to main menu
					ShutdownNetwork();
					DeinitializeBoard();
					_currentState = GameState::MAIN_MENU;
					// call main menu handler
					stateFunctions[_currentState]();
					return;
				}
				// Retry match
				if (retryBtn.getGlobalBounds().contains(mfp))
				{
					// restart singleplayer match
					DeinitializeBoard();
					InitializeBoard();
					// Enter game loop again
					Update();
					return;
				}
			}
			// allow Esc to return to main menu
			if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				DeinitializeBoard();
				_currentState = GameState::MAIN_MENU;
				stateFunctions[_currentState]();
				return;
			}
		}

		_window.clear();
		// draw current board behind (optional) then overlay modal
		DrawGame();
		_window.draw(backg);
		_window.draw(label);
		_window.draw(backBtn);
		_window.draw(retryBtn);
		_window.draw(backTxt);
		_window.draw(retryTxt);
		_window.display();
	}
}

void GameManager::SettingsMenu()
{
	cout << "Settings Menu (not implemented yet)...\n";
	// Placeholder for settings menu logic
	_currentState = GameState::CLOSED;
	stateFunctions[_currentState]();
}

void GameManager::PausedMenu()
{
	// Modal frame layout (matching EndGameMenu style)
	const float itemSize = 180.f;
	const float padding = 40.f;
	const float menuWidth = (itemSize * 2.f) + padding * 3.f; // two buttons + paddings
	const float menuHeight = itemSize + padding * 4.f;        // label + buttons area

	sf::Vector2u winSize = _window.getSize();
	float menuX = (float(winSize.x) - menuWidth) / 2.f;
	float menuY = (float(winSize.y) - menuHeight) / 2.f;

	sf::RectangleShape backg(sf::Vector2f(menuWidth, menuHeight));
	backg.setPosition(sf::Vector2f(menuX, menuY));
	backg.setFillColor(sf::Color(30, 30, 30, 230));
	backg.setOutlineColor(sf::Color::White);
	backg.setOutlineThickness(3.f);

	// Load font
	sf::Font font;
	const string fontPath = "Assets/Fonts/arial.ttf";
	if (!font.openFromFile(fontPath))
		cerr << "Failed to load font: " << fontPath << "\n";

	// Label "Paused"
	sf::Text label(font);
	label.setString("PAUSED");
	label.setCharacterSize(34u);
	label.setFillColor(sf::Color::White);
	label.setStyle(sf::Text::Bold);

	// Correct label positioning for SFML 3 (use .position / .size)
	{
		sf::FloatRect lb = label.getLocalBounds(); // lb.position.x/y, lb.size.x/y
		// center horizontally inside modal and set top at menuY + padding
		float labelX = menuX + (menuWidth - lb.size.x) / 2.f - lb.position.x;
		float labelY = menuY + padding - lb.position.y;
		label.setPosition(sf::Vector2f(labelX, labelY));
	}

	// Buttons: RESUME (left) and LEAVE MATCH (right)
	float btnW = itemSize;
	float btnH = itemSize * 0.5f;

	// Compute button Y based on the label's size and position
	sf::FloatRect lb2 = label.getLocalBounds();
	float btnY = (menuY + padding - lb2.position.y) + lb2.size.y + padding;

	sf::RectangleShape resumeBtn(sf::Vector2f(btnW, btnH));
	sf::RectangleShape leaveBtn(sf::Vector2f(btnW, btnH));
	resumeBtn.setPosition(sf::Vector2f(menuX + padding, btnY));
	leaveBtn.setPosition(sf::Vector2f(menuX + padding * 2.f + btnW, btnY));

	resumeBtn.setFillColor(sf::Color(70, 70, 70, 220));
	leaveBtn.setFillColor(sf::Color(70, 70, 70, 220));
	resumeBtn.setOutlineColor(sf::Color::White);
	leaveBtn.setOutlineColor(sf::Color::White);
	resumeBtn.setOutlineThickness(1.f);
	leaveBtn.setOutlineThickness(1.f);

	sf::Text resumeTxt(font);
	resumeTxt.setString("RESUME");
	resumeTxt.setCharacterSize(20u);
	sf::Text leaveTxt(font);
	leaveTxt.setString("LEAVE MATCH");
	leaveTxt.setCharacterSize(20u);
	resumeTxt.setFillColor(sf::Color::White);
	leaveTxt.setFillColor(sf::Color::White);

	// helper to center text in button (SFML 3 API)
	auto centerTextInRect = [&](sf::Text& t, const sf::RectangleShape& r)
	{
		sf::FloatRect tb = t.getLocalBounds();    // tb.position.x/y, tb.size.x/y
		sf::FloatRect rb = r.getGlobalBounds();   // rb.position.x/y, rb.size.x/y
		float tx = rb.position.x + (rb.size.x - tb.size.x) / 2.f - tb.position.x;
		float ty = rb.position.y + (rb.size.y - tb.size.y) / 2.f - tb.position.y;
		t.setPosition(sf::Vector2f(tx, ty));
	};
	centerTextInRect(resumeTxt, resumeBtn);
	centerTextInRect(leaveTxt, leaveBtn);

	// Modal loop
	std::optional<sf::Event> event;
	bool done = false;
	while (_window.isOpen() && !done)
	{
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				DeinitializeBoard();
				_window.close();
				return;
			}

			// Mouse click handling
			if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));

				if (resumeBtn.getGlobalBounds().contains(mfp))
					return;// Resume: simply close the modal and return to Update()

				if (leaveBtn.getGlobalBounds().contains(mfp))
				{
					// Leave match: cleanup and go to main menu
					DeinitializeBoard();
					ShutdownNetwork();
					_currentState = GameState::MAIN_MENU;
					stateFunctions[_currentState]();
					return;
				}
			}

			// Allow Esc or P to resume
			if (event->is<sf::Event::KeyPressed>() &&
				(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)))
			{
				// resume
				return;
			}
		}

		// Draw underlying game, then overlay modal
		_window.clear();
		DrawGame();
		_window.draw(backg);
		_window.draw(label);
		_window.draw(resumeBtn);
		_window.draw(leaveBtn);
		_window.draw(resumeTxt);
		_window.draw(leaveTxt);
		_window.display();
	}
}

void GameManager::HostGame()
{
	cout << "Hosting a Game...\n";

	// Start ENet server
	if (!_networkMgr.InitializeHostAsServer(7777))
	{
		cerr << "Failed to start server\n";
		_currentState = GameState::MAIN_MENU;
		stateFunctions[_currentState]();
		return;
	}

    _isHosting = true; // set the role flag
	_localIsWhite = true;
	_awaitingServer = false;

	// Launch network service thread
	_networkThread = std::thread(&NetworkManager::ServiceLoop, &_networkMgr);

	// Prepare modal UI (simple centered text + Cancel button)
	sf::Font font;
	const string fontPath = "Assets/Fonts/arial.ttf";
	if (!font.openFromFile(fontPath))
		cerr << "Failed to load font: " << fontPath << "\n";

	const float backW = 520.f;
	const float backH = 220.f;
	sf::Vector2u winSize = _window.getSize();
	float backX = (float(winSize.x) - backW) / 2.f;
	float backY = (float(winSize.y) - backH) / 2.f;

	sf::RectangleShape back(sf::Vector2f(backW, backH));
	back.setPosition(sf::Vector2f(backX, backY));
	back.setFillColor(sf::Color(30, 30, 30, 230));
	back.setOutlineColor(sf::Color::White);
	back.setOutlineThickness(3.f);

	sf::Text label(font);
	label.setString("Waiting for client to connect...");
	label.setCharacterSize(24u);
	label.setFillColor(sf::Color::White);
	{
		sf::FloatRect lb = label.getLocalBounds();
		label.setPosition(sf::Vector2f(backX + (backW - lb.size.x) / 2.f - lb.position.x,
			backY + 30.f - lb.position.y));
	}

	// Cancel button
	const float btnW = 160.f, btnH = 52.f;
	sf::RectangleShape cancelBtn(sf::Vector2f(btnW, btnH));
	cancelBtn.setPosition(sf::Vector2f(backX + (backW - btnW) / 2.f, backY + backH - btnH - 24.f));
	cancelBtn.setFillColor(sf::Color(70, 70, 70, 220));
	cancelBtn.setOutlineColor(sf::Color::White);
	cancelBtn.setOutlineThickness(1.f);

	sf::Text cancelTxt(font);
	cancelTxt.setString("CANCEL");
	cancelTxt.setCharacterSize(20u);
	cancelTxt.setFillColor(sf::Color::White);
	{
		sf::FloatRect tb = cancelTxt.getLocalBounds();
		sf::FloatRect rb = cancelBtn.getGlobalBounds();
		cancelTxt.setPosition(sf::Vector2f(
			rb.position.x + (rb.size.x - tb.size.x) / 2.f - tb.position.x,
			rb.position.y + (rb.size.y - tb.size.y) / 2.f - tb.position.y
		));
	}

	// Modal loop: returns only when client connects or user cancels
	std::optional<sf::Event> event;
	bool done = false;
	while (_window.isOpen() && !done)
	{
		while (event = _window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				// user closed window: ensure network stops then exit
				DeinitializeBoard();
				ShutdownNetwork();
				_window.close();
				return;
			}

			if (event->is<sf::Event::MouseMoved>())
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				if (cancelBtn.getGlobalBounds().contains(mfp))
					cancelBtn.setFillColor(sf::Color(100, 100, 100, 240));
				else
					cancelBtn.setFillColor(sf::Color(70, 70, 70, 220));
			}

			if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				sf::Vector2i mp = sf::Mouse::getPosition(_window);
				sf::Vector2f mfp(static_cast<float>(mp.x), static_cast<float>(mp.y));
				if (cancelBtn.getGlobalBounds().contains(mfp))
				{
					// Cancel hosting and return to main menu
					ShutdownNetwork();
					_currentState = GameState::MAIN_MENU;
					stateFunctions[_currentState]();
					return;
				}
			}
		}

		// When client connects:
		if (_networkMgr.IsConnected())
		{
			cout << "Client connected — starting match.\n";
			// Start the game loop (server-authoritative)
			InitializeBoard();
			PlayGame(true);            // <-- run multiplayer mode
			ShutdownNetwork();
			_currentState = GameState::MAIN_MENU;
			stateFunctions[_currentState]();
			return;
		}

		// Draw modal
		_window.clear();
		DrawGame(); // optional background
		_window.draw(back);
		_window.draw(label);
		_window.draw(cancelBtn);
		_window.draw(cancelTxt);
		_window.display();

		// small sleep to avoid busy loop (keeps UI responsive)
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// If we get here, make sure network stopped
	ShutdownNetwork();
}

void GameManager::ConnectToGame()
{
	cout << "Connecting to a Game...\n";
	string host = "127.0.0.1";
	const uint16_t port = 7777;

	if (!_networkMgr.InitializeHostAsClient(host.c_str(), port))
	{
		cerr << "Failed to initialize client host.\n";
		// return to main menu
		_currentState = GameState::MAIN_MENU;
		stateFunctions[_currentState]();
		return;
	}

    _isHosting = false; // set role flag to client
	_localIsWhite = false;
	_awaitingServer = false;

	// Start network thread
	_networkThread = std::thread(&NetworkManager::ServiceLoop, &_networkMgr);

	// Wait short time for connect (non-UI blocking is preferable; we use console wait here)
	const int maxWaitMs = 3000;
	const int stepMs = 100;
	int waited = 0;
	cout << "Waiting for connection...\n";
	while (waited < maxWaitMs)
	{
		if (_networkMgr.IsConnected())
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(stepMs));
		waited += stepMs;
	}

	if (!_networkMgr.IsConnected())
	{
		cerr << "Connection timed out.\n";
		// Stop network and join thread
		ShutdownNetwork();
		// Go back to main menu
		_currentState = GameState::MAIN_MENU;
		stateFunctions[_currentState]();
		return;
	}

	cout << "Connected to server. Entering game loop.\n";
	// Start playing (you may want a dedicated multiplayer PlayGame)

	InitializeBoard();
    PlayGame(true); // <-- run multiplayer mode
}

void GameManager::PlayGame(bool isMultiplayer)
{
	cout << "Starting the Game...\n";
	// Placeholder for game loop logic
	InitializeBoard();
	Update(isMultiplayer);
}

