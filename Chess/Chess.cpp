// Chess.cpp : Defines the entry point for the application.
//
#include <vector>
#include <SFML/Graphics.hpp>


#include "Chess.h"
#include  "Tile.h"
#include "Figurines/Figurine.h"
#include "Figurines/Bishop.h"

using std::cout;
using std::endl;
using std::vector;

sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(sf::VideoMode({ 512,512 }, desktop.bitsPerPixel), "Chess Game", sf::Style::Close | sf::Style::Resize);
static bool currentRound = 0; // Variable to track the current round (white or black) 0 for white, 1 for black
static vector<Figurine*> p1Figures{}; // Vector to hold player 1's figures
static vector<Figurine*> p2Figures{}; // Vector to hold player 2's figures

//void DrawBoard(Tile tiles[8][8])
//{
//	sf::Color playFieldColor;
//	for (int i = 0; i < 8; i++)
//	{
//		for (int j = 0; j < 8; j++)
//		{
//			// Initialize each tile
//			tiles[i][j] = Tile(i, j, false);
//			//Draw each tile from the array to the window
//			sf::RectangleShape tile(sf::Vector2f(64, 64));
//            tile.setPosition(sf::Vector2f(static_cast<float>(i * 64), static_cast<float>(j * 64)));
//			if ((i + j) % 2 == 0)
//			{
//				playFieldColor = sf::Color(118,150,86); // Light green color
//				tile.setFillColor(playFieldColor);
//			}
//			else
//			{
//				playFieldColor = sf::Color(238,238,210); // Light beige color
//				tile.setFillColor(playFieldColor);
//			}
//			window.draw(tile);
//		}
//	}
//	window.display();
//}

void DrawBoard(Tile tiles[8][8])
{
    sf::Color playFieldColor;
    sf::Font font;
    // Load a font from file (make sure the font file exists in your Assets folder)
    if (!font.openFromFile("Assets/arial.ttf")) {
        // Handle error (font not found)
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // Initialize each tile
            tiles[i][j] = Tile(i, j, false);
            // Draw each tile from the array to the window
            sf::RectangleShape tile(sf::Vector2f(64, 64));
            tile.setPosition(sf::Vector2f(static_cast<float>(i * 64), static_cast<float>(j * 64)));
            if ((i + j) % 2 == 0)
            {
                playFieldColor = sf::Color(118, 150, 86); // Light green color
                tile.setFillColor(playFieldColor);
            }
            else
            {
                playFieldColor = sf::Color(238, 238, 210); // Light beige color
                tile.setFillColor(playFieldColor);
            }
            window.draw(tile);

            // Draw coordinates as string
            sf::Text coordText = sf::Text(font, "", 12);
            coordText.setFont(font);
            coordText.setString(std::to_string(i) + "," + std::to_string(j));
            coordText.setCharacterSize(16);
            coordText.setFillColor(sf::Color::Black);
            coordText.setPosition(sf::Vector2f(static_cast<float>(i * 64 + 4), static_cast<float>(j * 64 + 4)));
            window.draw(coordText);
        }
    }
    //window.display();
}

void setFigures(Tile tiles[8][8])
{
    
}


int main()
{
	Tile tiles[8][8];
	Bishop bishop(0, 0, true); // Create a Bishop figurine at position (0, 0) for player 1 (white)
    bishop.getSprite().setScale(sf::Vector2f(static_cast<float>(0.5f), static_cast<float>(0.5f))); // Scale the sprite to fit the window
    bishop.getSprite().setPosition(sf::Vector2f(static_cast<float>(0.f), static_cast<float>(0.f))); // Set the position of the sprite

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
		DrawBoard(tiles);
        window.draw(bishop.getSprite());
        window.display();

	}

	return 0;
	
}
