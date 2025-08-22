#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>
#include <filesystem>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;

class AssetManager
{
public:
    AssetManager();

    // Accessors
    const sf::Texture& getTexture(const string& key) const;
    sf::Sprite getSprite(const string& key) const;

private:
    map<string, sf::Texture> textures;
};
