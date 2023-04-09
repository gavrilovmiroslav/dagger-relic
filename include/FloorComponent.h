#pragma once

enum FloorType
{
    FT_Sand = 0,
    FT_QuickSand = 1
};

struct Floor
{
    FloorType type;

    Floor(FloorType type=FT_Sand)
    : type(type)
    {
    }
};