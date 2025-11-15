#pragma once
#include <vector>

enum TileID {
    // "Empty"
    TILE_AIR = -1,

    // Basic blocks / terrain
    TILE_GRAVEL = 0,
    TILE_STONE = 1,
    TILE_DIRT = 2,
    TILE_GRASS = 3,
    TILE_COBBLESTONE = 16,
    TILE_BEDROCK = 17,
    TILE_SAND = 18,

    // Logs & wood variants
    TILE_LOG = 20,
    TILE_DARK_LOG = 116,
    TILE_WHITE_LOG = 117,
    TILE_LEAVES = 153,

    // Building / decorative blocks
    TILE_BRICKS = 7,
    TILE_BOOKSHELF = 35,
    TILE_MOSSY_COBBLESTONE = 36,
    TILE_OBSIDIAN = 37,
    TILE_PLANKS = 4, 
    TILE_LAPIS_BLOCK = 144,
    TILE_CRAFTING_TABLE = 60,
    TILE_ENCHANTING_TABLE = 182,

    // Functional blocks
    TILE_CHEST = 27,
    TILE_FURNACE = 44,

    // Ores
    TILE_GOLD_ORE = 32,
    TILE_IRON_ORE = 33,
    TILE_COAL_ORE = 34,
    TILE_DIAMOND_ORE = 50,
    TILE_RUBY_ORE = 51,
    TILE_LAPIS_ORE = 160,

    // Solid metal / gem blocks
    TILE_IRON_BLOCK = 22,
    TILE_GOLD_BLOCK = 23,
    TILE_DIAMOND_BLOCK = 24,

    // Misc / interactables
    TILE_TNT = 8,
    TILE_TORCH = 80,
    TILE_PUMPKIN = 102,
    TILE_MELON = 137,
    TILE_CAKE = 140,

    // Plants / flora
    TILE_FLOWER_RED = 12,
    TILE_FLOWER_YELLOW = 13,
    TILE_MUSHROOM_RED = 28,
    TILE_MUSHROOM_BROWN = 29,

    // Liquids
    TILE_WATER = 207,
    TILE_LAVA = 210,

    // Count sentinel (keeps original position/index)
    TILE_COUNT,

    // Tools (kept in same enum for backward compatibility with existing code)
    TOOL_SWORD = 204,
    TOOL_PICKAXE = 203,
    TOOL_AXE = 1002,
    TOOL_SHOVEL = 1003
};

// inline vectors you can use at runtime to check membership.
inline const std::vector<int> TILE_CATEGORY_TERRAIN = {
    TILE_GRAVEL, TILE_STONE, TILE_DIRT, TILE_GRASS, TILE_SAND, TILE_COBBLESTONE, TILE_BEDROCK
};

inline const std::vector<int> TILE_CATEGORY_WOOD = {
    TILE_LOG, TILE_DARK_LOG, TILE_WHITE_LOG, TILE_LEAVES, TILE_PLANKS
};

inline const std::vector<int> TILE_CATEGORY_ORES = {
    TILE_GOLD_ORE, TILE_IRON_ORE, TILE_COAL_ORE, TILE_DIAMOND_ORE, TILE_RUBY_ORE, TILE_LAPIS_ORE, TILE_IRON_BLOCK, TILE_GOLD_BLOCK,
    TILE_DIAMOND_BLOCK
};

inline const std::vector<int> TILE_CATEGORY_DECORATIVE = {
    TILE_BRICKS, TILE_BOOKSHELF, TILE_MOSSY_COBBLESTONE, TILE_OBSIDIAN, TILE_LAPIS_BLOCK, TILE_CRAFTING_TABLE, TILE_ENCHANTING_TABLE,
    TILE_CHEST, TILE_FURNACE, TILE_TNT, TILE_TORCH, TILE_PUMPKIN, TILE_MELON, TILE_CAKE,
};

inline const std::vector<int> TILE_CATEGORY_PLANTS = {
    TILE_FLOWER_RED, TILE_FLOWER_YELLOW, TILE_MUSHROOM_RED, TILE_MUSHROOM_BROWN, TILE_PUMPKIN, TILE_MELON
};

inline const std::vector<int> TILE_CATEGORY_LIQUIDS = {
    TILE_WATER, TILE_LAVA
};

inline const std::vector<int> ITEM_CATEGORY_TOOLS = {
    TOOL_SWORD, TOOL_PICKAXE, TOOL_AXE, TOOL_SHOVEL
};


