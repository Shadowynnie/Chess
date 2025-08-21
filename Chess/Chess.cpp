// Chess.cpp : Defines the entry point for the application.
//
#include <vector>
#include <SFML/Graphics.hpp>

#include "GameManager.h"
#include "Chess.h"
#include  "Tile.h"
#include "Figurines/Figure.h"
#include "Figurines/Bishop.h"
#include "Figurines/King.h"
#include "Figurines/Knight.h"
#include "Figurines/Rook.h"
#include "Figurines/Queen.h"
#include "Figurines/Pawn.h"

using std::cout;
using std::endl;
using std::vector;

int main()
{

	GameManager gameManager;
	gameManager.InitializeBoard();
	gameManager.Update();
	return 0;
}
