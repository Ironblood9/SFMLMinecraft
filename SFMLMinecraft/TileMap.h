#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    bool load(const std::string& tileset, // file path
              sf::Vector2u tileSize,     //  square size(exp:{32,32})
              const std::vector<int>& tiles, //holding the tile index(0 - based) for each cell in the tileset
              unsigned int width, unsigned int height);

    void setTile(unsigned int x, unsigned int y, int tileId);
    int  getTile(unsigned int x, unsigned int y) const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void updateTileVertices(unsigned int x, unsigned int y);

    sf::VertexArray         map_vertices;
    sf::Texture             map_tileset;
    std::vector<int>        map_tiles;
    unsigned int            map_width = 0;
    unsigned int            map_height = 0;
    sf::Vector2u            map_tileSize;
};

