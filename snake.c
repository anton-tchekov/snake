#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>

#define FIELD_WIDTH         40
#define FIELD_HEIGHT        40
#define BLOCK_SIZE          20

#define WINDOW_WIDTH          (FIELD_WIDTH * BLOCK_SIZE)
#define WINDOW_HEIGHT         (FIELD_HEIGHT * BLOCK_SIZE)

#define WINDOW_TITLE          "Snake"
#define SNAKE_MAXLEN        128
#define SNAKE_INITLEN         8

enum DIR
{
	UP = 1,
	DOWN,
	LEFT,
	RIGHT
};

typedef enum
{
	RUNNING = 1
}
Status;

typedef struct SNAKE_BLOCK
{
	int x, y;
} SnakeBlock;

typedef struct SNAKE
{
	SnakeBlock blocks[SNAKE_MAXLEN];
	int len;
} Snake;

typedef struct FOOD
{
	int x, y;
} Food;

static int _ctor(SDL_Window **window, SDL_Renderer **renderer);
static void _dtor(SDL_Window *window, SDL_Renderer *renderer);

static void _draw_snake(SDL_Renderer *renderer, Snake *snake);
static void _draw_food(SDL_Renderer *renderer, Food *food);
static void _random_food(Snake *snake, Food *food);
static int checkdir(int dir, Snake *snake);

int main(int argc, char **argv)
{
	int i, dir = 0;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Event event;
	/* Current Piece, Next Piece */
	Status status;
	Snake snake;
	Food food;
	int ticks, last_ticks = 0, ticks_update = 100;

	srand(time(0));

	snake.len = SNAKE_INITLEN;
	for(i = 0; i < SNAKE_INITLEN; ++i)
	{
		snake.blocks[i].x = i;
		snake.blocks[i].y = 0;
	}

	_random_food(&snake, &food);

	if(_ctor(&window, &renderer))
	{
		return 1;
	}

	status = RUNNING;
	while(status & RUNNING)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					status &= ~RUNNING;
					break;

				/*case SDLK_DOWN:
					dir = DOWN;
					break;

				case SDLK_UP:
					dir = UP;
					break;

				case SDLK_LEFT:
					dir = LEFT;
					break;

				case SDLK_RIGHT:
					dir = RIGHT;
					break;*/

				default:
					break;
				}
				break;

			case SDL_QUIT:
				status &= ~RUNNING;
				break;
			}
		}

		/* White clear */
		SDL_SetRenderDrawColor(renderer,
			255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		_draw_snake(renderer, &snake);
		_draw_food(renderer, &food);

		if((ticks = SDL_GetTicks()) > last_ticks + ticks_update)
		{
			last_ticks = ticks;
			
			if(food.x > snake.blocks[snake.len - 1].x)
			{
				dir = RIGHT;
			}

			if(food.y > snake.blocks[snake.len - 1].y)
			{
				dir = DOWN;
			}

			if(food.x < snake.blocks[snake.len - 1].x)
			{
				dir = LEFT;
			}

			if(food.y < snake.blocks[snake.len - 1].y)
			{
				dir = UP;
			}
			
			if(checkdir(dir, &snake))
			{
				for(dir = 1; dir < 4; ++dir)
				{
					if(!checkdir(dir, &snake))
					{
						break;
					}
				}
			}

			if(dir)
			{
				for(i = 0; i < snake.len - 1; ++i)
				{
					snake.blocks[i].x = snake.blocks[i + 1].x;
					snake.blocks[i].y = snake.blocks[i + 1].y;
				}

				switch(dir)
				{
				case UP:
					--snake.blocks[snake.len - 1].y;
					break;

				case DOWN:
					++snake.blocks[snake.len - 1].y;
					break;

				case LEFT:
					--snake.blocks[snake.len - 1].x;
					break;

				case RIGHT:
					++snake.blocks[snake.len - 1].x;
					break;
				}

				if(
					snake.blocks[snake.len - 1].x < 0 || snake.blocks[snake.len - 1].x > FIELD_WIDTH ||
					snake.blocks[snake.len - 1].y < 0 || snake.blocks[snake.len - 1].y > FIELD_HEIGHT)
				{
					goto exit;
				}

				for(i = 0; i < snake.len - 1; ++i)
				{
					if(
						snake.blocks[snake.len - 1].x == snake.blocks[i].x &&
						snake.blocks[snake.len - 1].y == snake.blocks[i].y)
					{
						goto exit;
					}
				}

				if(food.x == snake.blocks[snake.len - 1].x &&
					food.y == snake.blocks[snake.len - 1].y)
				{
					if(snake.len + 1 < SNAKE_MAXLEN)
					{
						++snake.len;
						snake.blocks[snake.len - 1].x = snake.blocks[snake.len - 2].x;
						snake.blocks[snake.len - 1].y = snake.blocks[snake.len - 2].y;
						_random_food(&snake, &food);
					}
				}
			}
		}

		SDL_RenderPresent(renderer);
	}

exit:
	printf("Game Over\n");
	_dtor(window, renderer);
	return 0;
}

static void _draw_snake(SDL_Renderer *renderer, Snake *snake)
{
	SDL_Rect r;
	r.w = BLOCK_SIZE;
	r.h = BLOCK_SIZE;
	int i;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for(i = 0; i < snake->len; ++i)
	{
		r.x = snake->blocks[i].x * BLOCK_SIZE;
		r.y = snake->blocks[i].y * BLOCK_SIZE;
		SDL_RenderFillRect(renderer, &r);
	}
}

static void _draw_food(SDL_Renderer *renderer, Food *food)
{
	SDL_Rect r;
	r.x = food->x * BLOCK_SIZE;
	r.y = food->y * BLOCK_SIZE;
	r.w = BLOCK_SIZE;
	r.h = BLOCK_SIZE;
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &r);
}

static void _random_food(Snake *snake, Food *food)
{
	int i;
	food->x = -1;
	while(food->x == -1)
	{
		food->x = rand() % FIELD_WIDTH;
		food->y = rand() % FIELD_HEIGHT;
		for(i = 0; i < snake->len; ++i)
		{
			if(food->x == snake->blocks[i].x || food->y == snake->blocks[i].y)
			{
				food->x = -1;
				break;
			}
		}
	}
}

static int checkdir(int dir, Snake *snake)
{
	int x = snake->blocks[snake->len - 1].x;
	int y = snake->blocks[snake->len - 1].y;
	int i;
	switch(dir)
	{
	case UP:
		--y;
		break;

	case DOWN:
		++y;
		break;

	case LEFT:
		--x;
		break;

	case RIGHT:
		++x;
		break;
	}

	if(x < 0 || y < 0 || x >= FIELD_WIDTH || y >= FIELD_HEIGHT)
	{
		return 1;
	}

	for(i = 0; i < snake->len - 1; ++i)
	{
		if(x == snake->blocks[i].x && y == snake->blocks[i].y)
		{
			return 1;
		}
	}

	return 0;
}

static int _ctor(SDL_Window **window, SDL_Renderer **renderer)
{
	int error = 0;

	enum
	{
		ERROR_INIT_SDL        = 1,
		ERROR_CREATE_WINDOW   = 2,
		ERROR_CREATE_RENDERER = 3
	};

	do
	{
		/* Init SDL */
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Error initializing SDL; SDL_Init: %s\n",
				SDL_GetError());
			error = ERROR_INIT_SDL;
			break;
		}

		/* Create SDL_Window */
		if((*window = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT, 0)) == NULL)
		{
			printf("Error creating SDL_Window: %s\n",
				SDL_GetError());
			error = ERROR_CREATE_WINDOW;
			break;
		}

		/* Create SDL_Renderer */
		if((*renderer = SDL_CreateRenderer
			(*window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
		{
			printf("Error creating SDL_Renderer: %s\n",
				SDL_GetError());
			error = ERROR_CREATE_RENDERER;
			break;
		}
	}
	while(0);

	switch(error)
	{
		case ERROR_CREATE_RENDERER:
			SDL_DestroyWindow(*window);

		case ERROR_CREATE_WINDOW:
			SDL_Quit();

		case ERROR_INIT_SDL:
			return 1;
	}

	SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
	return 0;
}

static void _dtor(SDL_Window *window, SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
