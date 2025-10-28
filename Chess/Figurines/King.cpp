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
			(target->GetFigure() && target->GetFigure()->GetColor() != IsWhite))
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
        // Directon according to color
		int row = IsWhite ? 0 : 7;

        // Short castling (to the right side)
		Tile* rookTileKingside = &tiles[7][row];
		Figure* rookKingside = rookTileKingside->GetFigure();
		if (rookKingside && typeid(*rookKingside) == typeid(Rook))
		{
			Rook* rook = dynamic_cast<Rook*>(rookKingside);
			if (rook && !rook->HasMoved())
			{
                // Tiles between king and rook must be unoccupied
				if (!tiles[5][row].IsOccupied() && !tiles[6][row].IsOccupied())
				{
                    // The king must not be in check, pass through or land on threatened squares
					bool safePath = true;
					for (int x = 4; x <= 6; ++x)
					{
						X = x; Y = row;
						if (IsThreatened(tiles, enemyFigures))
						{
							safePath = false;
							break;
						}
					}
                    X = 4; Y = row; // return to original position
					if (safePath)
                        possibleMoves.push_back(&tiles[6][row]); // King's target position
				}
			}
		}

        // Long castling (to the left side)
		Tile* rookTileQueenside = &tiles[0][row];
		Figure* rookQueenside = rookTileQueenside->GetFigure();
		if (rookQueenside && typeid(*rookQueenside) == typeid(Rook))
		{
			Rook* rook = dynamic_cast<Rook*>(rookQueenside);
			if (rook && !rook->HasMoved())
			{
				if (!tiles[1][row].IsOccupied() &&
					!tiles[2][row].IsOccupied() &&
					!tiles[3][row].IsOccupied())
				{
					bool safePath = true;
					for (int x = 2; x <= 4; ++x)
					{
						X = x; Y = row;
						if (IsThreatened(tiles, enemyFigures))
						{
							safePath = false;
							break;
						}
					}
					X = 4; Y = row;
					if (safePath)
                        possibleMoves.push_back(&tiles[2][row]); // King's target position
				}
			}
		}
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

    // If the king moved two squares horizontally, it's a castling move
	if (std::abs(deltaX) == 2)
	{
		if (deltaX > 0)
		{
            // Short castling (kingside)
			Tile* rookTile = &tiles[7][row];
			Figure* rookFigure = rookTile->GetFigure();
			Rook* rook = dynamic_cast<Rook*>(rookFigure);

			if (rook && !rook->HasMoved() && !_hasMoved)
                rookFigure->Move(&tiles[5][row], rookTile, tiles);				
		}
		else
		{
            // Long castling (queenside)
			Tile* rookTile = &tiles[0][row];
			Figure* rookFigure = rookTile->GetFigure();
			Rook* rook = dynamic_cast<Rook*>(rookFigure);

			if (rook && !rook->HasMoved() && !_hasMoved)
				rookFigure->Move(&tiles[3][row], rookTile, tiles);
		}
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