#pragma once

float fsignf(float x)
{
	if (x > 0.0f){
        return  1.0f;
    }
	if (x < 0.0f){
        return -1.0f;
    }
	else{
        return  0.0f;
    }
}

float fclampf(float x, float min, float max)
{
	if (x < min){
        return min;
    } 
	if (x > max){
        return max;
    } 
	else{         
        return x;
    }
}
