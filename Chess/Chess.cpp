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

    /*
	while(window.isOpen())
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



        window.display();

	}
    */
	return 0;
}
