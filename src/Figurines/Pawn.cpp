#include "Figurines/Pawn.h"

// Includes for promotion
#include "Figurines/Queen.h"
#include "Figurines/Bishop.h"
#include "Figurines/Rook.h"

using std::remove;

Pawn::Pawn(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	string key = IsWhite ? "pawn_white" : "pawn_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile*> Pawn::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	int direction = IsWhite ? 1 : -1;

	auto inBounds = [](int x, int y)
		{
			return x >= 0 && x < 8 && y >= 0 && y < 8;
		};

	auto tryAddTile = [&](int x, int y)
		{
			if (inBounds(x, y))
				possibleMoves.push_back(&tiles[x][y]);
		};

	auto canCapture = [&](int x, int y)
		{
			if (!inBounds(x, y))
				return false;
			if (!tiles[x][y].IsOccupied())
				return false;
			return tiles[x][y].GetFigure()->GetColor() != IsWhite;
		};

	// --- Forward move ---
	int forwardY = Y + direction;
	if (inBounds(X, forwardY) && !tiles[X][forwardY].IsOccupied())
	{
		tryAddTile(X, forwardY);

		// Double step
		int startRow = IsWhite ? 1 : 6;
		int doubleStepY = Y + 2 * direction;
		if (Y == startRow && !tiles[X][doubleStepY].IsOccupied())
			tryAddTile(X, doubleStepY);
	}

	// --- Diagonal captures (both sides) ---
	for (int dx : {-1, 1})
	{
		int nx = X + dx;
		int ny = Y + direction;
		if (canCapture(nx, ny))
			tryAddTile(nx, ny);
	}

	// --- En passant captures ---
	int enPassantRow = IsWhite ? 4 : 3;
	if (Y == enPassantRow)
	{
		auto checkEnPassant = [&](int dx)
			{
				int nx = X + dx;
				if (!inBounds(nx, Y)) return;

				Figure* neighbor = tiles[nx][Y].GetFigure();
				if (neighbor &&
					typeid(*neighbor) == typeid(Pawn) &&
					neighbor->GetColor() != IsWhite)
				{
					Pawn* pawn = dynamic_cast<Pawn*>(neighbor);
					if (pawn && pawn->IsEnPassantAvailable())
						tryAddTile(nx, Y + direction);
				}
			};
		checkEnPassant(-1); // left
		checkEnPassant(1);  // right
	}
	return possibleMoves;
}

bool Pawn::CanPromote() const
{
	// If a white pawn reaches the opposite end of the chessboard (y == 7)
	// or the black pawn reaches the opposite end of the chessboard (y == 0)
	return (IsWhite && Y == 7) || (!IsWhite && Y == 0);
}

void Pawn::EnableEnPassant(bool enable)
{
	_enPassantAvailable = enable;
}

bool Pawn::IsEnPassantAvailable() const
{
	return _enPassantAvailable;
}

// Overriden move function to handle en passant
void Pawn::Move(Tile* tile, Tile* previousTile, vector<Figure*>& enemyFigures, Tile tiles[8][8])
{
	int oldX = X;
	int oldY = Y;
	int newX = tile->GetX();
	int newY = tile->GetY();
	int direction = IsWhite ? 1 : -1;

	bool isDiagonal = (oldX != newX);
	bool isForward = (newY - oldY == direction);
	bool isDoubleStep = std::abs(newY - oldY) == 2;

	// --- En Passant Detection (check before normal move) ---
	bool enPassantCapture = false;
	Tile* capturedTile = nullptr;
	if (isDiagonal && isForward && !tile->IsOccupied())  // diagonal to empty tile
	{
		int capturedY = newY - direction;                // pawn is "behind" the destination
		capturedTile = &tiles[newX][capturedY];
		Figure* capturedFigure = capturedTile->GetFigure();

		if (capturedFigure &&
			typeid(*capturedFigure) == typeid(Pawn) &&
			capturedFigure->GetColor() != IsWhite)
		{
			Pawn* capturedPawn = dynamic_cast<Pawn*>(capturedFigure);
			if (capturedPawn && capturedPawn->_enPassantAvailable)
				enPassantCapture = true;
		}
	}

	// --- Perform the actual move ---
	Figure::Move(tile, previousTile);

	// --- Handle en passant capture ---
	if (enPassantCapture && capturedTile)
	{
		Figure* capturedFigure = capturedTile->GetFigure();
		if (capturedFigure)
		{
			auto it = std::find(enemyFigures.begin(), enemyFigures.end(), capturedFigure);
			if (it != enemyFigures.end())
			{
				enemyFigures.erase(it);
				delete capturedFigure;
				capturedTile->SetFigure(nullptr);
			}
		}
	}

	// --- Handle double-step flag ---
	_enPassantAvailable = isDoubleStep;
}