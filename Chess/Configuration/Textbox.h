#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Textbox
{
public:
    Textbox(float x, float y, float w, float h, sf::Font& fontRef, unsigned size = 22)
        : font(fontRef),
        text(fontRef, "", size)
    {
        box.setPosition({ x, y });
        box.setSize({ w, h });
        box.setFillColor(sf::Color(50, 50, 50));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);

        text.setFillColor(sf::Color::White);
        text.setPosition({ x + 8.f, y + (h - size) * 0.5f - 4.f });
    }

    void handleEvent(const sf::Event& e, const sf::Window& win)
    {
        // FOCUS CHECK
        if (e.is<sf::Event::MouseButtonPressed>())
        {
            auto mp = sf::Mouse::getPosition(win);
            focused = box.getGlobalBounds().contains(sf::Vector2f(mp));
            box.setOutlineColor(focused ? sf::Color::Yellow : sf::Color::White);
        }

        // TEXT INPUT
        if (focused)
        {
            if (auto t = e.getIf<sf::Event::TextEntered>())
            {
                uint32_t code = t->unicode;
                if (code == 8)  // Backspace
                {
                    if (!str.empty())
                        str.pop_back();
                }
                else if (code >= 32 && code < 128) // Printable ASCII
                    str.push_back(static_cast<char>(code));
                text.setString(str);
            }
        }
    }

    void draw(sf::RenderWindow& win)
    {
        win.draw(box);
        win.draw(text);
    }

    std::string getString() const { return str; }
    void setString(const std::string& s)
    {
        str = s;
        text.setString(str);
    }

private:
    sf::RectangleShape box;
    sf::Font& font;
    sf::Text text;
    bool focused = false;
    std::string str;
};
