#include "King.h"
#include "Rook.h"

King::King(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	string key = IsWhite ? "king_white" : "king_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

bool King::CanCastle(Tile tiles[8][8], vector<Figure*>& enemyFigures, int rookX, int targetKingX)
{
	if (_hasMoved)
		return false; // King already moved

	int row = IsWhite ? 0 : 7;

	Tile* rookTile = &tiles[rookX][row];
	Figure* rookFigure = rookTile->GetFigure();
	Rook* rook = (rookFigure && typeid(*rookFigure) == typeid(Rook))
		? dynamic_cast<Rook*>(rookFigure)
		: nullptr;

	if (!rook || rook->HasMoved())
		return false;

	// Determine step direction (right = +1, left = -1)
	int step = (rookX > X) ? 1 : -1;

	// Check empty tiles between king and rook
	for (int x = X + step; x != rookX; x += step)
	{
		if (tiles[x][row].IsOccupied())
			return false;
	}

	// Check if any of the king’s path squares are threatened
	for (int x = X; x != targetKingX + step; x += step)
	{
		X = x; Y = row;
		if (IsThreatened(tiles, enemyFigures))
		{
			X = 4; Y = row; // restore
			return false;
		}
	}
	X = 4; Y = row; // restore

	return true;
}

vector<Tile*> King::GetPossibleMoves(Tile tiles[8][8], vector<Figure*>& enemyFigures)
{
	vector<Tile*> possibleMoves;
	int kingMoves[8][2] = 
	{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};

	for (const auto& move : kingMoves)
	{
		int newX = X + move[0];
		int newY = Y + move[1];
		if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8)
			continue;

		Tile* target = &tiles[newX][newY];

        // King can move to an unoccupied tile or capture an enemy piece
		if (!target->IsOccupied() ||
			(target->GetFigure() &&
			target->GetFigure()->GetColor() != IsWhite))
		{
            // Simulating a move to check for threats
			int oldX = X;
			int oldY = Y;
			Figure* captured = target->GetFigure();

            // Temporarily move the king
			X = newX;
			Y = newY;
			target->SetFigure(this);

			bool threatened = IsThreatened(tiles, enemyFigures);

            // Return to original position
			X = oldX;
			Y = oldY;
			target->SetFigure(captured);

            // If not threatened, add to possible moves
			if (!threatened)
				possibleMoves.push_back(target);
		}
	}
	// --- Castling check ---
    if (!_hasMoved) // King has not moved yet
	{
		int row = IsWhite ? 0 : 7;

		// Kingside castling
		if (CanCastle(tiles, enemyFigures, 7, 6))
			possibleMoves.push_back(&tiles[6][row]);

		// Queenside castling
		if (CanCastle(tiles, enemyFigures, 0, 2))
			possibleMoves.push_back(&tiles[2][row]);
	}
	return possibleMoves;
}

vector<Tile*> King::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// King moves one square in any direction
	int kingMoves[8][2] = 
	{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};
	for (const auto& move : kingMoves)
	{
		int newX = X + move[0];
		int newY = Y + move[1];
		if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
		{
			if (!tiles[newX][newY].IsOccupied() ||
				(tiles[newX][newY].IsOccupied() &&
				 (tiles[newX][newY].GetFigure() != nullptr && 
				  tiles[newX][newY].GetFigure()->GetColor() != IsWhite)))
			{
				possibleMoves.push_back(&tiles[newX][newY]);
			}
		}
	}
	return possibleMoves;
}

// Overriden move function to handle castling moves
void King::Move(Tile* tile, Tile* previousTile, Tile tiles[8][8])
{
	int oldX = X;
	int oldY = Y;

	// Normal king move
	Figure::Move(tile, previousTile, tiles);

	int deltaX = X - oldX;
	int row = IsWhite ? 0 : 7;
    int col = oldY;

    // If the king moved two squares horizontally, it's a castling move
	if (std::abs(deltaX) == 2)
	{
		Tile* rookTile;
        Figure* rookFigure;
		Rook* rook;
		if (deltaX > 0)
		{
			col = 7;
			rookTile = &tiles[col][row];
			col = 5;				
		}
		else
		{
			// Long castling (queenside)
            col = 0;
			rookTile = &tiles[col][row];
            col = 3;
		}
		rookFigure = rookTile->GetFigure();
		rook = dynamic_cast<Rook*>(rookFigure);
		if (rook && !rook->HasMoved() && !_hasMoved)
			rookFigure->Move(&tiles[col][row], rookTile, tiles);
	}
    _hasMoved = true;
}

bool King::IsThreatened(Tile tiles[8][8], vector<Figure*>& enemyFigures)
{
	for (auto& enemy : enemyFigures)
	{
		vector<Tile*> enemyMoves = enemy->GetPossibleMoves(tiles);
		for (auto& tile : enemyMoves)
		{
			if (tile->GetX() == X && tile->GetY() == Y)
			{
				return true; // King is threatened
			}
		}
	}
	return false; // King is safe
}