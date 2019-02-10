#include <SDL.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <string>
#include <fstream>

static const int MAXITERATIONS = 255;

static const int WWIDTH = 3840;
static const int WHEIGHT = 2160;

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


void calculateQuarter(SDL_Renderer *renderer, int startx, int endx, int starty, int endy) {
	for (int x = startx; x < endx; x++)
		for (int y = starty; y < endy; y++) {
			double real = (x - WWIDTH / 2.0) * zoom + offsety;
			double imag = (y - WHEIGHT / 2.0) * zoom + offsetx;
			int iters = mandelbrot(real, imag);
			//std::cout << "iterations: " << iters << std::endl;
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}


void saveImage() {
	std::string filename = "test.ppm";
	std::cout << "enter filename: ";
	std::cin >> filename;
	std::ofstream outFile(filename);
	outFile << "P3\n";
	outFile << WWIDTH << " " << WHEIGHT << "\n";
	outFile << "255\n";

	for (int y = 0; y < WHEIGHT; y++) {
		
		for (int x = 0; x < WWIDTH; x++) {
			outFile << (int)screenbuffer[x][y].r << " " << (int)screenbuffer[x][y].g << " " << (int)screenbuffer[x][y].b << " ";
 		}
		outFile << "\n";
	}
	std::cout << "done saving...\n";
}


int main(int argc, char* argv[]) {
	//color table filling
	for (int i = 0; i < MAXITERATIONS; i++) {
		double normalizedIterations = (i / MAXITERATIONS) + 1;
		SDL_Color outcolor; outcolor.a = 255;
		

		SDL_Color colors[16];
		if (i < MAXITERATIONS && i > 0) {
			int n = i % 16;
			colors[0].r = 66; colors[0].g = 30; colors[0].b = 15; colors[0].a = 255;
			colors[1].r = 25; colors[1].g = 7; colors[1].b = 26; colors[1].a = 255;
			colors[2].r = 9; colors[2].g = 1; colors[2].b = 47; colors[2].a = 255;
			colors[3].r = 4; colors[3].g = 4; colors[3].b = 73; colors[3].a = 255;
			colors[4].r = 0; colors[4].g = 7; colors[4].b = 100; colors[4].a = 255;
			colors[5].r = 12; colors[5].g = 44; colors[5].b = 138; colors[5].a = 255;
			colors[6].r = 24; colors[6].g = 82; colors[6].b = 177; colors[6].a = 255;
			colors[7].r = 57; colors[7].g = 125; colors[7].b = 209; colors[7].a = 255;
			colors[8].r = 134; colors[8].g = 181; colors[8].b = 229; colors[8].a = 255;
			colors[9].r = 211; colors[9].g = 236; colors[9].b = 248; colors[9].a = 255;
			colors[10].r = 241; colors[10].g = 233; colors[10].b = 191; colors[10].a = 255;
			colors[11].r = 248; colors[11].g = 201; colors[11].b = 95; colors[11].a = 255;
			colors[12].r = 255; colors[12].g = 170; colors[12].b = 0; colors[12].a = 255;
			colors[13].r = 204; colors[13].g = 128; colors[13].b = 0; colors[13].a = 255;
			colors[14].r = 153; colors[14].g = 87; colors[14].b = 0; colors[14].a = 255;
			colors[15].r = 106; colors[15].g = 52; colors[15].b = 3; colors[15].a = 255;
			colorLookup[i] = colors[n];
		}
		else {
			colorLookup[i].r = 0; colorLookup[i].g = 0; colorLookup[i].b = 0;
		}


	}

	//end of color table filling

	//window handling
	SDL_Window *window = SDL_CreateWindow("Mandelbrot set renderer in SDL | cstamas2k", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WWIDTH / 4, WHEIGHT / 4, SDL_WINDOW_SHOWN);
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
			case SDLK_s: saveImage(); break;
			default: needRedraw = false; break;
			}
			system("cls");
		}

		if (needRedraw) {
			std::cout << "Zoom level: " << zoom << std::endl;
			std::cout << "X offset: " << offsetx << std::endl;
			std::cout << "Y offset: " << offsety << std::endl;

			std::vector<std::thread> threads;

			threads.push_back(std::thread(calculateQuarter, renderer, 0, WWIDTH / 2, 0, WHEIGHT / 2)); //top left
			threads.push_back(std::thread(calculateQuarter, renderer, WWIDTH / 2, WWIDTH, 0, WHEIGHT / 2)); //top right
			threads.push_back(std::thread(calculateQuarter, renderer, 0, WWIDTH / 2, WHEIGHT / 2, WHEIGHT)); //bottom left
			threads.push_back(std::thread(calculateQuarter, renderer, WWIDTH / 2, WWIDTH, WHEIGHT / 2, WHEIGHT)); //bottom right

			for (auto &t : threads) {
				t.join();
			}

			for (int x = 0; x < WWIDTH; x += 4)
				for (int y = 0; y < WHEIGHT; y +=4) {
						SDL_SetRenderDrawColor(renderer, screenbuffer[x][y].r, screenbuffer[x][y].g, screenbuffer[x][y].b, 255);
						SDL_RenderDrawPoint(renderer, x / 4, y / 4);
				}

			needRedraw = false;
			std::cout << "done!\n";
		}

		SDL_RenderPresent(renderer);
	}

	return 0;
}
