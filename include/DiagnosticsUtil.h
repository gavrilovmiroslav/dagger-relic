#pragma once

#include "Definitions.h"

using namespace std::chrono;

struct DiagEntry
{
	F32 value;
	high_resolution_clock::time_point sample_time;
};

enum DiagType
{
	DIAG_TYPE_FPS = 0,
	DIAG_TYPE_COUNT = 1
};

struct DiagnosticsSignal
{
	DiagType diagType;
	F32 value;
};

struct ScrollingBuffer
{
	int max_size;
	int offset;
	containers::DynamicArray<SDL_FPoint> Data;

	ScrollingBuffer(int size = 100)
	{
		max_size = size;
		offset = 0;
		Data.reserve(max_size);
	}

	void AddPoint(float x, float y)
	{
		if (Data.size() < max_size)
		{
			Data.push_back(SDL_FPoint{x, y});
		}
		else
		{
			Data[offset] = SDL_FPoint{x, y};
			offset = (offset + 1) % max_size;
		}
	}

	void Erase()
	{
		if (Data.size() > 0)
		{
			Data.clear();
			offset = 0;
		}
	}
};
