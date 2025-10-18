#include "TileMap.h"
#include "TileID.h"
#include <iostream>
#include <cmath>

bool TileMap::load(const std::string& tileset, sf::Vector2u tileSize,
    const std::vector<int>& tiles, unsigned int width, unsigned int height)
{
    if (width * height != tiles.size()) {
        std::cerr << "TileMap::load -> tiles vector size mismatch\n";
        return false;
    }


    sf::Image image;
    if (!image.loadFromFile(tileset)) {
        std::cerr << "Failed to load: " << tileset << "\n";
        return false;
    }


    if (!map_tileset.loadFromImage(image)) {
        std::cerr << "Failed to load texture from processed image!\n";
        return false;
    }




    // Texture'ı repeat modunda ayarla ve smooth'u kapat
    map_tileset.setRepeated(false);
    map_tileset.setSmooth(false);
    map_tileSize = tileSize;
    map_width = width;
    map_height = height;
    map_tiles = tiles;

    // Triangles 
    map_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    map_vertices.resize(static_cast<std::size_t>(width) * height * 6);

    for (unsigned int j = 0; j < height; ++j) {
        for (unsigned int i = 0; i < width; ++i) {
            updateTileVertices(i, j);
        }
    }

    return true;
}

void TileMap::updateTileVertices(unsigned int i, unsigned int j)
{
    // Get the tile index
    int tileNumber = map_tiles[i + j * map_width];

    // Get vertex index
    int vertexIndex = (i + j * map_width) * 6;

    // Air tile - make completely transparent
    if (tileNumber == TILE_AIR) {
        for (int k = 0; k < 6; ++k) {
            map_vertices[vertexIndex + k].color = sf::Color(0, 0, 0, 0);
            map_vertices[vertexIndex + k].texCoords = sf::Vector2f(0.f, 0.f);
        }
        return;
    }

    int tu = tileNumber % (map_tileset.getSize().x / map_tileSize.x);
    int tv = tileNumber / (map_tileset.getSize().x / map_tileSize.x);

    // Adjust vertex position and texture coordinate 
    float left = static_cast<float>(i * map_tileSize.x);
    float right = left + static_cast<float>(map_tileSize.x);
    float top = static_cast<float>(j * map_tileSize.y);
    float bottom = top + static_cast<float>(map_tileSize.y);

    float texLeft = static_cast<float>(tu * map_tileSize.x);
    float texRight = texLeft + static_cast<float>(map_tileSize.x);
    float texTop = static_cast<float>(tv * map_tileSize.y);
    float texBottom = texTop + static_cast<float>(map_tileSize.y);

    // Texture koordinatlarını 0.5 piksel içeri alarak kenar sorunlarını önle
    float texturePadding = 0.5f;
    texLeft += texturePadding;
    texRight -= texturePadding;
    texTop += texturePadding;
    texBottom -= texturePadding;

    // First triangle
    map_vertices[vertexIndex + 0].position = sf::Vector2f(left, top);
    map_vertices[vertexIndex + 1].position = sf::Vector2f(right, top);
    map_vertices[vertexIndex + 2].position = sf::Vector2f(left, bottom);

    map_vertices[vertexIndex + 0].texCoords = sf::Vector2f(texLeft, texTop);
    map_vertices[vertexIndex + 1].texCoords = sf::Vector2f(texRight, texTop);
    map_vertices[vertexIndex + 2].texCoords = sf::Vector2f(texLeft, texBottom);

    // Second triangle
    map_vertices[vertexIndex + 3].position = sf::Vector2f(right, top);
    map_vertices[vertexIndex + 4].position = sf::Vector2f(right, bottom);
    map_vertices[vertexIndex + 5].position = sf::Vector2f(left, bottom);

    map_vertices[vertexIndex + 3].texCoords = sf::Vector2f(texRight, texTop);
    map_vertices[vertexIndex + 4].texCoords = sf::Vector2f(texRight, texBottom);
    map_vertices[vertexIndex + 5].texCoords = sf::Vector2f(texLeft, texBottom);

    // Set all vertex colors to white with full opacity
    for (int k = 0; k < 6; ++k) {
        map_vertices[vertexIndex + k].color = sf::Color::White;
    }
}

// Diğer fonksiyonlar aynı kalacak...
int TileMap::getTile(unsigned int x, unsigned int y) const
{
    if (x >= map_width || y >= map_height) return -1;
    return map_tiles[x + y * map_width];
}

void TileMap::setTile(unsigned int i, unsigned int j, int tile)
{
    if (i < map_width && j < map_height) {
        map_tiles[i + j * map_width] = tile;
        updateTileVertices(i, j);
    }
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &map_tileset;
    states.blendMode = sf::BlendAlpha;
    target.draw(map_vertices, states);
}