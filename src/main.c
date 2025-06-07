#include "wasm4.h"
#include "snake.h"

#include <stdlib.h>

void snake_create(Snake* snake) {
    if (snake->body != NULL) {
        free(snake->body);
        snake->body = NULL;
    }
    snake->length = 0;
}

void snake_push(Snake* snake, Point p) {
    Point* body = realloc(snake->body, sizeof(Point) * (snake->length+1));
    if (body) {
        snake->body = body;
        snake->body[snake->length++] = p;
    }
}

void snake_update(Snake* snake) {
    //update positions for everything except head
    for (size_t i=snake->length-1; i >= 1; i--) {
        snake->body[i].x = snake->body[i-1].x;
        snake->body[i].y = snake->body[i-1].y;
    }
    //update head position
    const int new_x = snake->body[0].x + snake->direction.x;
    const int new_y = snake->body[0].y + snake->direction.y;
    snake->body[0].x = ((new_x % 20) + 20) % 20; //snake wraps
    snake->body[0].y = ((new_y % 20) + 20) % 20;
}

void snake_draw(Snake* snake) {
    //head
    *DRAW_COLORS = 0x0032;
    rect(snake->body[0].x * 8, snake->body[0].y * 8, 8, 8);
    
    //rest of body
    *DRAW_COLORS = 0x0003;
    for (size_t i=1; i < snake->length; i++) {
        rect(snake->body[i].x * 8, snake->body[i].y * 8, 8, 8);
    }
}

int snake_collision(Snake* snake) {
    for (size_t i=4; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            return 1;
        }
    }
    return 0;
}

int fruit_handler(Point* fruit, Snake* snake) {
    //draw the fruit
    *DRAW_COLORS = 0x0004;
    rect(fruit->x * 8, fruit->y * 8, 8, 8);

    //move the fruit if collected and extend the snake
    if (snake->body[0].x == fruit->x && snake->body[0].y == fruit->y) {
        *fruit = (Point) {rand() % 20, rand() % 20};

        size_t last_i = snake->length-1;
        snake_push(snake, (Point){snake->body[last_i].x, snake->body[last_i].y});

        return 1;
    }
    return 0;
}

uint8_t prev_pressed = 0;
void input(const uint8_t* gamepad, Snake* snake) {
    const uint8_t just_pressed = *gamepad & (*gamepad ^ prev_pressed);
    prev_pressed = *gamepad;

    if (snake->direction.y == 0) {
        if (BUTTON_UP & just_pressed) {
            snake->direction = (Point){0,-1};
        }
        else if (BUTTON_DOWN & just_pressed) {
            snake->direction = (Point){0,1};
        }
    }
    else if (snake->direction.x == 0) {
        if (BUTTON_LEFT & just_pressed) {
            snake->direction = (Point){-1,0};
        }
        else if (BUTTON_RIGHT & just_pressed) {
            snake->direction = (Point){1,0};
        }
    }
}


Snake snake;
Point fruit;
int score = 0;
int speed = 15;
int frame_count = 1;
int game_over = 0;
void start() {
    PALETTE[0] = 0x000000; //background
    PALETTE[1] = 0x426e5d; //snake head
    PALETTE[2] = 0x2c4a3e; //rest of snake
    PALETTE[3] = 0x2c384a; //pickups
    
    //make snake
    snake_create(&snake);
    snake_push(&snake, (Point){3,0});
    snake_push(&snake, (Point){2,0});
    snake_push(&snake, (Point){1,0});
    snake_push(&snake, (Point){0,0});
    snake.direction = (Point){1,0};

    //make fruit
    fruit = (Point){rand() % 20, rand() % 20};
}
void update () {
    if (game_over) {
        text("Game over", 42, 20);
    }
    else {
        //input logic
        input(GAMEPAD1, &snake);

        //logic for moving and speed up the snake over time
        if (frame_count % speed == 0) {
            snake_update(&snake);
            if (snake_collision(&snake)) {
                game_over = 1;
            }
        }
        if (frame_count % 750 == 0 && speed >= 5) {
            speed--; //actually increases speed
        }
        //fruit logic
        score += fruit_handler(&fruit, &snake);

        //drawing
        snake_draw(&snake);
        char score_text[20];
        //sprintf(score_text, "Score: %d", score);
        //text(score_text, 10, 10);
    }
    frame_count++;
}
