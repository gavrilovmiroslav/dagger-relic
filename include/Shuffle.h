#pragma once 

#include "Prelude.h"

template <typename T>
void randomize(DynamicArray<T>& elements)
{
    srand(time(0));

    U32 n = elements.size();
    for (U32 i = n - 1; i > 0; --i)
    {
        U32 j = rand() % (i + 1);
        std::swap(elements[i], elements[j]);
    }
};