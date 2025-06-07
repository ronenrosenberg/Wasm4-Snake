#pragma once
#include <stdlib.h>

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Snake {
    Point* body;
    Point direction;
    size_t length;
} Snake;

// void snake_create(Snake *snake);
// void snake_push(Snake *snake, Point p);