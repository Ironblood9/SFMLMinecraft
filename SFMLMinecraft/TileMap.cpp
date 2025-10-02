#include "TileMap.h"
#include <iostream>
#include <cmath>

bool TileMap::load(const std::string& tileset, sf::Vector2u tileSize,
    const std::vector<int>& tiles, unsigned int width, unsigned int height)
{
    if (width * height != tiles.size()) {
        std::cerr << "TileMap::load -> tiles vector size mismatch\n";
        return false;
    }

    if (!map_tileset.loadFromFile(tileset)) {
        std::cerr << "TileMap::load -> failed to load tileset: " << tileset << "\n";
        return false;
    }

    map_tileset.setSmooth(false);
    map_tileSize = tileSize;
    map_width = width;
    map_height = height;
    map_tiles = tiles;

    //Triangles 
    map_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    map_vertices.resize(static_cast<std::size_t>(width) * height * 6); // 6 vertex = 2 Triangles

    for (unsigned int j = 0; j < height; ++j) {
        for (unsigned int i = 0; i < width; ++i) {
            updateTileVertices(i, j);
        }
    }

    return true;
}

void TileMap::updateTileVertices(unsigned int x, unsigned int y)
{
    if (x >= map_width || y >= map_height) return;
    unsigned int index = (x + y * map_width) * 6; // 6 vertex per tile
    int tileNumber = map_tiles[x + y * map_width];

    unsigned int tsCols = map_tileset.getSize().x / map_tileSize.x;
    if (tsCols == 0) return;

    int tu = tileNumber % static_cast<int>(tsCols);
    int tv = tileNumber / static_cast<int>(tsCols);

    // World positions
    sf::Vector2f pos0(static_cast<float>(x * map_tileSize.x), static_cast<float>(y * map_tileSize.y));
    sf::Vector2f pos1(static_cast<float>((x + 1) * map_tileSize.x), static_cast<float>(y * map_tileSize.y));
    sf::Vector2f pos2(static_cast<float>((x + 1) * map_tileSize.x), static_cast<float>((y + 1) * map_tileSize.y));
    sf::Vector2f pos3(static_cast<float>(x * map_tileSize.x), static_cast<float>((y + 1) * map_tileSize.y));

    // Texture coordinate
    sf::Vector2f tex0(static_cast<float>(tu * map_tileSize.x), static_cast<float>(tv * map_tileSize.y));
    sf::Vector2f tex1(static_cast<float>((tu + 1) * map_tileSize.x), static_cast<float>(tv * map_tileSize.y));
    sf::Vector2f tex2(static_cast<float>((tu + 1) * map_tileSize.x), static_cast<float>((tv + 1) * map_tileSize.y));
    sf::Vector2f tex3(static_cast<float>(tu * map_tileSize.x), static_cast<float>((tv + 1) * map_tileSize.y));

    // Triangle 1: pos0, pos1, pos2
    map_vertices[index].position = pos0;
    map_vertices[index].texCoords = tex0;
    map_vertices[index].color = sf::Color::White;

    map_vertices[index + 1].position = pos1;
    map_vertices[index + 1].texCoords = tex1;
    map_vertices[index + 1].color = sf::Color::White;

    map_vertices[index + 2].position = pos2;
    map_vertices[index + 2].texCoords = tex2;
    map_vertices[index + 2].color = sf::Color::White;

    // Triangle 2: pos0, pos2, pos3
    map_vertices[index + 3].position = pos0;
    map_vertices[index + 3].texCoords = tex0;
    map_vertices[index + 3].color = sf::Color::White;

    map_vertices[index + 4].position = pos2;
    map_vertices[index + 4].texCoords = tex2;
    map_vertices[index + 4].color = sf::Color::White;

    map_vertices[index + 5].position = pos3;
    map_vertices[index + 5].texCoords = tex3;
    map_vertices[index + 5].color = sf::Color::White;
}

int TileMap::getTile(unsigned int x, unsigned int y) const
{
    if (x >= map_width || y >= map_height) return -1;
    return map_tiles[x + y * map_width];
}

void TileMap::setTile(unsigned int x, unsigned int y, int tileId)
{
    if (x >= map_width || y >= map_height) return;
    map_tiles[x + y * map_width] = tileId;
    updateTileVertices(x, y);
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &map_tileset;
    target.draw(map_vertices, states);
}

