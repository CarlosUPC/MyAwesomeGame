#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#pragma comment(lib, "../SDL/lib/x86/SDL2.lib")
#pragma comment(lib, "../SDL/lib/x86/SDL2main.lib")
#pragma comment(lib, "../SDL_Image/lib/x86/SDL2_image.lib")
#pragma comment(lib, "../SDL_Mixer/lib/x86/SDL2_mixer.lib")

//SDL Introducción
/*
const int WIDTH = 800, HEIGHT = 600;


int main(int argc, char *argv[])
{

	SDL_Surface *imageSurface = NULL;
	SDL_Surface *windowSurface = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL could not initialise! SDL Error: " << SDL_GetError() << std::endl;
	}

	SDL_Window *window = SDL_CreateWindow("The Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
	windowSurface = SDL_GetWindowSurface(window);

	if (NULL == window) {
		std::cout << "SDL could not create window! SDL Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cout << "SDL could not initialize SDL_Image: SDL Error: " << IMG_GetError() << std::endl;
		return 1;
	}

	SDL_Rect posicion_mario;
	
	posicion_mario.x = 50;
	posicion_mario.y = 50;
	posicion_mario.w = 250;
	posicion_mario.h = 250;

	SDL_Event windowEvent;

	imageSurface = IMG_Load("mario.png");

	if (NULL == imageSurface) {
		std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;
	}
	while (true)
	{

		while (SDL_PollEvent(&windowEvent))
		{
			if (SDL_QUIT == windowEvent.type)
			{
				return false;
			}
			else if (windowEvent.type == SDL_KEYDOWN) {
				if (windowEvent.key.keysym.sym == SDLK_ESCAPE) {
					exit(0);
				}
				if (windowEvent.key.keysym.sym == SDLK_UP) {
					posicion_mario.y -= 4;
				}
				if (windowEvent.key.keysym.sym == SDLK_DOWN) {
					posicion_mario.y += 4;
				}
				if (windowEvent.key.keysym.sym == SDLK_RIGHT) {
					posicion_mario.x += 4;
				}
				if (windowEvent.key.keysym.sym == SDLK_LEFT) {
					posicion_mario.x -= 4;
				}
			}
		}

		SDL_FillRect(windowSurface, 0, SDL_MapRGB(windowSurface->format, 0,150,0));

		SDL_BlitSurface(imageSurface, NULL, windowSurface, &posicion_mario);
		SDL_UpdateWindowSurface( window );
		
		
	}

	SDL_FreeSurface(imageSurface);
	SDL_FreeSurface(windowSurface);

	imageSurface = NULL;
	windowSurface = NULL;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;


}
*/


#define LENGTH 640
#define HEIGHT 480
#define SHIP_SPEED 1
#define SHOT_SPEED 1
#define SPACESHIP_X 64
#define SPACESHIP_Y 64
#define LASER_X 64
#define LASER_Y 64

typedef struct {

	int x, y;
	bool isAlive;
} Projectile;

typedef struct {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* ship = nullptr;
	SDL_Texture* backGround = nullptr;
	SDL_Texture* shot = nullptr;
	Mix_Music* music = nullptr;
	Mix_Chunk* audio = nullptr;
	int shipX = 0, shipY = 0;
	bool movLeft, movRight, movDown, movUp = false;
	Projectile shots;
} Globals;

Globals g;


void Start() {

	SDL_Init(SDL_INIT_VIDEO);

	// Creation of window and renderer
	g.window = SDL_CreateWindow("MiJuego", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LENGTH, HEIGHT, 0);
	g.renderer = SDL_CreateRenderer(g.window, -1, 0);

	// Load images
	IMG_Init(IMG_INIT_PNG);
	g.backGround = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("Sprites/background2.png"));
	g.ship = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("Sprites/ship.png"));
	g.shot = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("Sprites/shot.png"));

	//Load music
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);

	g.music = Mix_LoadMUS("zizibum.ogg");
	g.audio = Mix_LoadWAV("laser.wav");

	// Play music
	Mix_PlayMusic(g.music, -1);

	// Variable initialization
	g.shipX = 250;
	g.shipY = HEIGHT / 2;
	g.movDown = g.movUp = g.movLeft = g.movRight = false;
}


void Finish() {


	SDL_DestroyTexture(g.backGround);
	SDL_DestroyTexture(g.ship);
	SDL_DestroyTexture(g.shot);
	IMG_Quit();

	SDL_DestroyWindow(g.window);
	SDL_DestroyRenderer(g.renderer);

	Mix_FreeMusic(g.music);
	Mix_FreeChunk(g.audio);
	Mix_CloseAudio();
	Mix_Quit();

	SDL_Quit();
}


bool CheckInput() {

	// If an event is detected it will return true
	bool ret = true;
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {

		if (event.type == SDL_KEYUP) {

			switch (event.key.keysym.sym) {

			case SDLK_LEFT:
				g.movLeft = false;
				break;
			case SDLK_RIGHT:
				g.movRight = false;
				break;
			case SDLK_DOWN:
				g.movDown = false;
				break;
			case SDLK_UP:
				g.movUp = false;
				break;
			default:
				break;
			}
		}
		else if (event.type == SDL_KEYDOWN) {

			switch (event.key.keysym.sym) {

			case SDLK_LEFT:
				g.movLeft = true;
				break;
			case SDLK_RIGHT:
				g.movRight = true;
				break;
			case SDLK_DOWN:
				g.movDown = true;
				break;
			case SDLK_UP:
				g.movUp = true;
				break;
			case SDLK_x:
				ret = false;
				break;
			case SDLK_SPACE:
				if (!g.shots.isAlive) {    // The player can only shot when the shot is not alive
					Mix_PlayChannel(-1, g.audio, 0);
					g.shots.x = g.shipX;
					g.shots.y = g.shipY;

					g.shots.isAlive = true;
				}
				break;
			default:
				break;
			}
		}
		else if (event.type == SDL_QUIT) {

			ret = false;
		}
	}

	return ret;
}

void MoveStuff() {

	// Update ship position
	if (g.movLeft) g.shipX -= SHIP_SPEED;
	if (g.movRight) g.shipX += SHIP_SPEED;
	if (g.movDown) g.shipY += SHIP_SPEED;
	if (g.movUp) g.shipY -= SHIP_SPEED;

	// Cannot exceed the screen limits
	if (g.shipX == 0) g.shipX += 1;
	if (g.shipX == LENGTH - SPACESHIP_X) g.shipX -= 1;
	if (g.shipY == 0) g.shipY += 1;
	if (g.shipY == HEIGHT - SPACESHIP_Y) g.shipY -= 1;

	if (!g.shots.isAlive) {

		g.shots.x = g.shipX;
		g.shots.y = g.shipY;
	}
	else {

		if (g.shots.x < LENGTH) {

			g.shots.x += SHOT_SPEED;
		}
		else {

			g.shots.x = g.shipX;
			g.shots.y = g.shipY;

			g.shots.isAlive = false;
		}
	}
}

void Draw() {

	// Textures are putted into this rect in order to show the texture
	SDL_Rect sprite;

	// Background
	sprite = {

		0,
		0,
		LENGTH,
		HEIGHT
	};

	SDL_RenderCopy(g.renderer, g.backGround, NULL, &sprite);

	// Laser
	if (g.shots.isAlive) {

		sprite = {

			g.shots.x,
			g.shots.y,
			LASER_X,
			LASER_Y
		};

		SDL_RenderCopy(g.renderer, g.shot, NULL, &sprite);
	}

	// Ship
	sprite = {

		g.shipX,
		g.shipY,
		SPACESHIP_X,
		SPACESHIP_Y
	};

	SDL_RenderCopy(g.renderer, g.ship, NULL, &sprite);

	SDL_RenderPresent(g.renderer); // Render present is needed in order to show new sprites / changes of the screen
}

int main(int argc, char *argv[])
{

	Start();

	while (CheckInput()) {

		MoveStuff();
		Draw();
	}

	Finish();
	
	return 0;
}
