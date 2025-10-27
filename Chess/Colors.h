#pragma once
#include <SFML/Graphics/Color.hpp>

namespace Colors
{
    // Basic colors for the chessboard tiles
    inline const sf::Color LightTile = sf::Color(238, 238, 210); // light-beige
    inline const sf::Color DarkTile = sf::Color(118, 150, 86);  // dark-green

    // Highlight (for check condition)
    inline const sf::Color InCheckTile = sf::Color(173, 58, 41); // red (King in check)

    // Highlighting of possible moves (transparent overlay)
    inline const sf::Color MoveHighlight = sf::Color(255, 0, 0, 128); // semitransparent red

    // Colors for special indications (optional)
    inline const sf::Color LastMoveTile = sf::Color(246, 246, 105); // yellow shade (last turn)
    inline const sf::Color AvailableMove = sf::Color(30, 144, 255, 100); // blue shade for available moves

    // Edges or grids (if you would like to draw a frame)
    inline const sf::Color BoardBorder = sf::Color(50, 50, 50);

    // Player colors (for UI, etc.)
    inline const sf::Color WhitePlayer = sf::Color::White;
    inline const sf::Color BlackPlayer = sf::Color(40, 40, 40);
}
