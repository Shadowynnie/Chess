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
    // Accessors
    static const sf::Texture& getTexture(const string& key);
    static sf::Sprite getSprite(const string& key);
    static void loadTextures();

private:
	static map<string, string> fileNames;
    static map<string, sf::Texture> textures;
};
