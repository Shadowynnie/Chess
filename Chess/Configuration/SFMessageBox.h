#pragma once
#include <SFML/Graphics.hpp>

class SFMessageBox
{
public:
    static void Show(sf::RenderWindow& window, sf::Font& font, const std::string& msg)
    {
        sf::Vector2u win = window.getSize();

        const float W = 400.f, H = 200.f;
        float X = (win.x - W) / 2.f;
        float Y = (win.y - H) / 2.f;

        sf::RectangleShape box({ W, H });
        box.setPosition({ X, Y });
        box.setFillColor(sf::Color(30, 30, 30, 240));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(3);

        sf::Text text(font, msg, 22);
        text.setFillColor(sf::Color::White);

        auto tb = text.getLocalBounds();
        text.setPosition
        ({
            X + (W - tb.size.x) / 2.f - tb.position.x,
            Y + 40.f
        });

        // OK button
        sf::RectangleShape okBtn({ 120, 50 });
        okBtn.setFillColor(sf::Color(70, 70, 70));
        okBtn.setPosition({ X + (W - 120) / 2.f, Y + H - 70 });

        sf::Text okTxt(font, "OK", 22);
        auto ob = okTxt.getLocalBounds();
        okTxt.setPosition
        ({
            okBtn.getPosition().x + (120 - ob.size.x) / 2.f - ob.position.x,
            okBtn.getPosition().y + (50 - ob.size.y) / 2.f - ob.position.y
        });

        // Modal loop
        while (window.isOpen())
        {
            std::optional<sf::Event> e;
            while (e = window.pollEvent())
            {
                if (e->is<sf::Event::Closed>())
                {
                    window.close();
                    return;
                }
                if (e->is<sf::Event::MouseButtonPressed>())
                {
                    sf::Vector2f mp(sf::Mouse::getPosition(window));
                    if (okBtn.getGlobalBounds().contains(mp))
                        return;  // close message box
                }
            }
            window.clear(sf::Color(20, 20, 20));
            window.draw(box);
            window.draw(text);
            window.draw(okBtn);
            window.draw(okTxt);
            window.display();
        }
    }
};