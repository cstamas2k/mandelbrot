#include <SDL.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>

static const int MAXITERATIONS = 127;

static const int WWIDTH = 1920;
static const int WHEIGHT = 1080;

SDL_Color colorLookup[MAXITERATIONS];

double zoom = 0.004;
double offsetx = 0.00;
double offsety = 0.00;

SDL_Color screenbuffer[WWIDTH][WHEIGHT];


int mandelbrot(double startReal, double startImag) {
	double zReal = startReal;
	double zImag = startImag;

	for (int counter = 0; counter < MAXITERATIONS; ++counter) {
		double r2 = zReal * zReal;
		double i2 = zImag * zImag;
		if (r2 + i2 > 4.0) {
			return counter;
		}
		zImag = 2.0 * zReal * zImag + startImag;
		zReal = r2 - i2 + startReal;
	}
	return MAXITERATIONS;
}

void topLeft(SDL_Renderer *renderer) {
	for (int x = 0; x < WWIDTH / 2; x++)
		for (int y = 0; y < WHEIGHT / 2; y++) {
			double real = (x - WWIDTH / 2.0) * zoom + offsety;
			double imag = (y - WHEIGHT / 2.0) * zoom + offsetx;
			int iters = mandelbrot(real, imag);
			//std::cout << "iterations: " << iters << std::endl;
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}

void bottomRight(SDL_Renderer *renderer) {
	for (int x = WWIDTH / 2; x < WWIDTH; x++)
		for (int y = WHEIGHT / 2; y < WHEIGHT; y++) {
			double real = (x - WWIDTH / 2.0) * zoom + offsety;
			double imag = (y - WHEIGHT / 2.0) * zoom + offsetx;
			int iters = mandelbrot(real, imag);
			//std::cout << "iterations: " << iters << std::endl;
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}

void topRight(SDL_Renderer *renderer) {
	for (int x = WWIDTH / 2; x < WWIDTH; x++)
		for (int y = 0; y < WHEIGHT / 2; y++) {
			double real = (x - WWIDTH / 2.0) * zoom + offsety;
			double imag = (y - WHEIGHT / 2.0) * zoom + offsetx;
			int iters = mandelbrot(real, imag);
			//std::cout << "iterations: " << iters << std::endl;
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}

void bottomLeft(SDL_Renderer *renderer) {
	for (int x = 0; x < WWIDTH / 2; x++)
		for (int y = WHEIGHT / 2; y < WHEIGHT; y++) {
			double real = (x - WWIDTH / 2.0) * zoom + offsety;
			double imag = (y - WHEIGHT / 2.0) * zoom + offsetx;
			int iters = mandelbrot(real, imag);
			//std::cout << "iterations: " << iters << std::endl;
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}


int main(int argc, char* argv[]) {
	for (int i = 0; i < MAXITERATIONS; i++) {
		int r, g, b;
		if (i == 0) {
			r = 255;
			g = 0;
			b = 0;
		}
		else if (i < 16) {
			r = 16 * (16 - i);
			g = 0;
			b = 16 * (16 - i);
		}
		else if (i < 32) {
			g = 32 * (i - 32);
			r = 0;
			b = 32 * (32 - i) - 1;
		}
		else if (i < 64) {
			r = 64 * (i - 64);
			g = 64 * (64 - i) - 1;
			b = 0;
		}
		else {
			r = 255 - (i - 64) * 4;
			g = 0;
			b = 0;
		}

		SDL_Color returnColor;
		returnColor.r = r; returnColor.g = g; returnColor.b = b; returnColor.a = 255;
		if (r > 255) returnColor.r = 255;
		if (g > 255) returnColor.g = 255;
		if (b > 255) returnColor.b = 255;
		colorLookup[i] = returnColor;
	}

	//window handling
	SDL_Window *window = SDL_CreateWindow("Mandelbrot set renderer in SDL | cstamas2k", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WWIDTH, WHEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	bool shouldRun = true;
	bool needRedraw = true;
	
	while (shouldRun) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) shouldRun = false;

		if (event.type == SDL_KEYDOWN) {
			needRedraw = true;
			switch (event.key.keysym.sym) {
			case SDLK_i: zoom *= 0.9; break;
			case SDLK_k: zoom /= 0.9; break;

			case SDLK_UP: offsetx -= 40 * zoom; break;
			case SDLK_DOWN: offsetx += 40 * zoom; break;
			case SDLK_LEFT: offsety -= 40 * zoom; break;
			case SDLK_RIGHT: offsety += 40 * zoom; break;
			default: needRedraw = false; break;
			}
			system("cls");
		}

		if (needRedraw) {
			std::cout << "Zoom level: " << zoom << std::endl;
			std::cout << "X offset: " << offsetx << std::endl;
			std::cout << "Y offset: " << offsety << std::endl;
			
			std::vector<std::thread> threads;

			threads.push_back(std::thread(topLeft, renderer));
			threads.push_back(std::thread(topRight, renderer));
			threads.push_back(std::thread(bottomLeft, renderer));
			threads.push_back(std::thread(bottomRight, renderer));

			for (auto &t : threads) {
				t.join();
			}

			for (int x = 0; x < WWIDTH; x++)
				for (int y = 0; y < WHEIGHT; y++) {
					SDL_SetRenderDrawColor(renderer, screenbuffer[x][y].r, screenbuffer[x][y].g, screenbuffer[x][y].b, 255);
					SDL_RenderDrawPoint(renderer, x, y);
				}
			

			needRedraw = false;
			std::cout << "done!\n";
		}


		SDL_RenderPresent(renderer);
	}

	return 0;
}
