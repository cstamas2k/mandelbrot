#include <SDL.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <complex>

static const int MAXITERATIONS = 1023;
static const long double WWIDTH = 640;
static const long double WHEIGHT = 480;

SDL_Color colorLookup[MAXITERATIONS];
SDL_Color screenbuffer[640][480];

long double zoom = 1.004;
long double offsetx = -0.01;
long double offsety = 0.01;




int mandelbrot(std::complex<long double> inComplex) {
	std::complex<long double> myComplex = inComplex;

	for (int counter = 0; counter < MAXITERATIONS; ++counter) {

		std::complex<long double> myComplex2(myComplex.real() * myComplex.real(), myComplex.imag() * myComplex.imag());

		if (myComplex2.real() + myComplex2.imag() > 4.0L) {
			return counter;
		}

		myComplex = std::complex<long double>(myComplex2.real() - myComplex2.imag() + inComplex.real(), 2.0L * myComplex.real() * myComplex.imag() + inComplex.imag());
	}
	return MAXITERATIONS;
}


void calculateSegment(SDL_Renderer *renderer, int startx, int endx, int starty, int endy) {
	for (int x = startx; x < endx; x++)
		for (int y = starty; y < endy; y++) {
			std::complex<long double> mandelComplex((long double)(x - WWIDTH / 2.0L) / zoom + offsety, (long double)(y - WHEIGHT / 2.0L) / zoom + offsetx);
			int iters = mandelbrot(mandelComplex);
			SDL_Color rcolor = colorLookup[iters];
			screenbuffer[x][y] = rcolor;
		}
}


void saveImage(std::string filename) {
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
}

void debugPixel() {
	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);

	std::complex<long double> mandelComplex((mousex - WWIDTH / 2.0) / zoom + offsety, (mousey - WHEIGHT / 2.0) / zoom + offsetx);
	int iters = mandelbrot(mandelComplex);
	
	std::cout << "DEBUG: \n iterations: " << (int)iters << "\n complex number: " << mandelComplex << "\n";
	system("pause");
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
	SDL_Window *window = SDL_CreateWindow("Mandelbrot set renderer in SDL | cstamas2k", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WWIDTH, WHEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	bool shouldRun = true;
	bool needRedraw = true;
	bool ignoreZoomAtSpeed = false;
	bool doOutZoom = false;

	while (shouldRun) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) shouldRun = false;

		int movePixels = 2;
		if (ignoreZoomAtSpeed) movePixels = 8;
		if (event.type == SDL_KEYDOWN) {
			needRedraw = true;
			std::string file;
			switch (event.key.keysym.sym) {
			case SDLK_i: zoom *= 0.9; break;
			case SDLK_k: zoom /= 0.9; break;

			case SDLK_UP: offsetx -= (1 / zoom) * movePixels; break;
			case SDLK_DOWN: offsetx += (1 / zoom) * movePixels; break;
			case SDLK_LEFT: offsety -= (1 / zoom) * movePixels; break;
			case SDLK_RIGHT: offsety += (1 / zoom) * movePixels; break;
			case SDLK_s: std::cout << "enter filename: "; std::cin >> file; saveImage(file); break;
			case SDLK_LSHIFT: ignoreZoomAtSpeed = !ignoreZoomAtSpeed; break;
			case SDLK_d: debugPixel(); break;
			case SDLK_g: doOutZoom = true; break;
			default: needRedraw = false; break;
			}
			system("cls");
		}


		while (doOutZoom && zoom > 10) {
			saveImage(std::to_string(zoom));
			zoom *= 0.9;
			needRedraw = true;
			std::cout << "done " << std::to_string(zoom) << "...\n";
		}

		if (zoom <= 10) {
			doOutZoom = false;
		}

		if (needRedraw) {
			std::cout << "ignore moving speed: " << ignoreZoomAtSpeed << "\n";
			std::cout << "Zoom level: " << zoom << std::endl;
			std::cout << "X offset: " << offsetx << std::endl;
			std::cout << "Y offset: " << offsety << std::endl;

			std::vector<std::thread> threads;

			threads.push_back(std::thread(calculateSegment, renderer, 0, WWIDTH / 2, 0, WHEIGHT / 2)); //top left
			threads.push_back(std::thread(calculateSegment, renderer, WWIDTH / 2, WWIDTH, 0, WHEIGHT / 2)); //top right
			threads.push_back(std::thread(calculateSegment, renderer, 0, WWIDTH / 2, WHEIGHT / 2, WHEIGHT)); //bottom left
			threads.push_back(std::thread(calculateSegment, renderer, WWIDTH / 2, WWIDTH, WHEIGHT / 2, WHEIGHT)); //bottom right

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
