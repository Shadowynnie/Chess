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
        // Background
        box.setPosition({ x, y });
        box.setSize({ w, h });
        box.setFillColor(sf::Color(50, 50, 50));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);

        // Text
        text.setFillColor(sf::Color::White);
        text.setPosition({ x + 8.f, y + (h - size) * 0.5f - 4.f });

        // Caret (blinking line)
        caret.setSize({ 2.f, (float)size });
        caret.setFillColor(sf::Color::White);
        caret.setPosition(text.getPosition());
    }

    void setOutlineColor(const sf::Color& color)
    {
        box.setOutlineColor(color);
    }

    void handleEvent(const sf::Event& e, const sf::Window& win)
    {
        // Mouse click -> focus
        if (e.is<sf::Event::MouseButtonPressed>())
        {
            auto mp = sf::Mouse::getPosition(win);
            focused = box.getGlobalBounds().contains(sf::Vector2f(mp));
            box.setOutlineColor(focused ? sf::Color::Yellow : sf::Color::White);
        }

        // Text entry
        if (focused)
        {
            if (auto t = e.getIf<sf::Event::TextEntered>())
            {
                uint32_t code = t->unicode;

                if (code == 8)   // Backspace
                {
                    if (!str.empty())
                        str.pop_back();
                }
                else if (code >= 32 && code < 128) // Printable ASCII
                {
                    str.push_back(static_cast<char>(code));
                }

                text.setString(str);
                updateCaretPosition();
            }
        }
    }

    void update(float dt)
    {
        if (focused)
        {
            caretTimer += dt;
            if (caretTimer >= 0.5f)
            {
                caretVisible = !caretVisible;
                caretTimer = 0;
            }
        }
        else
        {
            caretVisible = false;
        }
    }

    void draw(sf::RenderWindow& win)
    {
        win.draw(box);
        win.draw(text);

        if (focused && caretVisible)
            win.draw(caret);
    }

    // Accessors
    std::string getString() const { return str; }
    void setString(const std::string& s)
    {
        str = s;
        text.setString(str);
        updateCaretPosition();
    }

private:
    void updateCaretPosition()
    {
        auto bounds = text.getLocalBounds();
        caret.setPosition(
            sf::Vector2f(text.getPosition().x + bounds.size.x - bounds.position.x + 2.f,
            text.getPosition().y
        ));
    }

    sf::RectangleShape box;
    sf::Font& font;
    sf::Text text;

    sf::RectangleShape caret;
    bool caretVisible = true;
    float caretTimer = 0.f;

    bool focused = false;
    std::string str;
};
