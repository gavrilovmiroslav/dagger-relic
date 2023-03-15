#pragma once

#include "Definitions.h"
#include "Access.h"

struct SDL_Texture;

struct Texture
{
	memory::RawPtr<SDL_Texture> inner;
};

