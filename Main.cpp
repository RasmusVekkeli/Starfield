//Fucking Windows storing libraries wrong.
#ifdef _WIN32 
	#include <SDL.h>
#else
	#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <ctime>
#include <chrono>
#include <string>

struct Star {
	float rotation;
	float distance;
};

struct Vector2 {
	int x;
	int y;
};

SDL_Window* gWnd;
SDL_Renderer* gRenderer;

long iWidth = 1920;
long iHeight = 1080;

long iStarCount = 1000;

float baseSpeed = 1000.f;

Vector2 v2ScreenMiddle = { iWidth / 2, iHeight / 2 };

std::vector<Star> starfield;

std::default_random_engine rng(time(0));
std::uniform_real_distribution<float> rndRot(0.f, 360.f);
std::uniform_real_distribution<float> rndDist(0.f, 30.f);

std::chrono::high_resolution_clock::time_point lastFrame;

bool init();
void close();

void update();
void draw();

Vector2 getCoordinates(Star star, Vector2 offset);

float getDeltaTime();

int main(int argc, char** argv) {
	if(argc > 1){
		iStarCount = std::stol(argv[1]);
	}

	if (init()) {
		for (size_t i = 0; i < iStarCount; i++) {
			starfield.push_back(Star{ rndRot(rng), rndDist(rng) });
		}

		bool quit = false;

		SDL_Event e;

		while (!quit) {
			lastFrame = std::chrono::high_resolution_clock::now();

			while (SDL_PollEvent(&e)) {
				switch (e.type)
				{
				case SDL_QUIT:
					quit = true;
					break;
				}
			}

			update();
			draw();
		}
	}

	close();
	return 0;
}

bool init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL_Init() failed: " << SDL_GetError() << std::endl;
	}
	else {
		gWnd = SDL_CreateWindow("Starfield", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, iWidth, iHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

		if (gWnd == 0) {
			std::cout << "SDL_CreateWindow() failed: " << SDL_GetError() << std::endl;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWnd, -1, SDL_RENDERER_ACCELERATED);

			if (gRenderer == 0) {
				std::cout << "SDL_CreateRenderer() failed: " << SDL_GetError() << std::endl;
			}
			else {
				return true;
			}
		}
	}

	return false;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	gRenderer = 0;

	SDL_DestroyWindow(gWnd);
	gWnd = 0;

	SDL_Quit();
}

void update() {
	float deltaTime = getDeltaTime();

	for (std::vector<Star>::iterator i = starfield.begin(); i < starfield.end(); std::advance(i, 1)) {
		i->distance = i->distance + (baseSpeed * deltaTime) * (i->distance * 0.1f);
	}
}

void draw() {
	SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(gRenderer);

	for (std::vector<Star>::iterator i = starfield.begin(); i < starfield.end(); std::advance(i, 1)) {
		Vector2 coords = getCoordinates(*i, v2ScreenMiddle);

		float t = (i->distance - 50) / 60;

		if (t > 1.f) {
			t = 1.f;
		}
		
		if (t < 0.f) {
			t = 0.f;
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF * t, 0xFF * t, 0xFF * t, 0xFF);

		if (coords.x < 0 || coords.x > iWidth || coords.y < 0 || coords.y > iHeight) {
			i->rotation = rndRot(rng);
			i->distance = rndDist(rng);
		}

		SDL_RenderDrawPoint(gRenderer, coords.x, coords.y);
	}

	SDL_RenderPresent(gRenderer);
}

Vector2 getCoordinates(Star star, Vector2 offset) {
	Vector2 ret;

	float tempY = sinf(star.rotation) * star.distance;
	ret.y = roundf(tempY);
	ret.x = (star.rotation > 180.f ? -1 : 1) * roundf(sqrtf(powf(star.distance, 2.f) - powf(tempY, 2.f)));

	ret.x += offset.x;
	ret.y += offset.y;

	return ret;
}

float getDeltaTime() {
	std::chrono::duration<float> dur = std::chrono::high_resolution_clock::now() - lastFrame;

	return dur.count();
}
