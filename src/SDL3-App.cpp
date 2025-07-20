#include <iostream>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

int main(int argc, char *argv[]) {
	glm::vec2 test = glm::vec2(2,2);
	SDL_Window* window;
	SDL_Texture* tex = IMG_LoadTexture(nullptr,"data/idle.png");
	SDL_Log("Working.");
}