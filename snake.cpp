#include <bits/stdc++.h>
using namespace std;

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_mixer.h>
#undef main

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 20;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event event;

int snakeX[100], snakeY[100];
int snakeLength = 1;
int direction = 0;  
int foodX, foodY;
int score = 0;

SDL_Texture* headRightTexture = nullptr;
SDL_Texture* headUpTexture = nullptr;
SDL_Texture* headLeftTexture = nullptr;
SDL_Texture* headDownTexture = nullptr;
SDL_Texture* bodyRightTexture = nullptr;
SDL_Texture* bodyUpTexture = nullptr;
SDL_Texture* bodyLeftTexture = nullptr;
SDL_Texture* bodyDownTexture = nullptr;
SDL_Texture* tailRightTexture = nullptr;
SDL_Texture* tailUpTexture = nullptr;
SDL_Texture* tailLeftTexture = nullptr;
SDL_Texture* tailDownTexture = nullptr;
SDL_Texture* foodTexture = nullptr;
SDL_Texture* bmpTexture = nullptr;
TTF_Font* font = nullptr;

Mix_Music* backgroundSound = nullptr;
Mix_Chunk* collideEffect = nullptr;
Mix_Chunk* laserEffect = nullptr;

bool running = true;
bool quit=true;


void renderMenu() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize SDL_image
    if (IMG_Init(IMG_INIT_JPG) == 0) {
        std::cout << "SDL_image Initialization Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        std::cout << "TTF Initialization Error: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Create a window and renderer for the menu
    SDL_Window* menuWindow = SDL_CreateWindow("Snake Game Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);

    // Load JPEG background image
    SDL_Surface* backgroundSurface = IMG_Load("images/fs.jpg");
    if (!backgroundSurface) {
        std::cout << "Image Loading Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(menuRenderer);
        SDL_DestroyWindow(menuWindow);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(menuRenderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    // Render the background
    SDL_RenderCopy(menuRenderer, backgroundTexture, NULL, NULL);
    SDL_DestroyTexture(backgroundTexture);

    // Text content and positions
    TTF_Font* menuFont = TTF_OpenFont("fonts/Roman SD.ttf", 36);
    SDL_Color textColor = {255, 0, 0, 255};

    std::string menuOptions[] = {"1.Start Game", "2.Exit", "3.About"};

    int lineHeight = 80;
    int yStartPos = 200;

    // Render and display each menu option
    for (const auto& option : menuOptions) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, option.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(menuRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        int xStartPos = (800 - textSurface->w) / 2;
        SDL_Rect destRect = {xStartPos, yStartPos, textSurface->w, textSurface->h};
        yStartPos += lineHeight;

        SDL_RenderCopy(menuRenderer, textTexture, NULL, &destRect);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(menuRenderer);

    // Wait for user input to close the menu
    bool menuRunning = true;
    SDL_Event menuEvent;
    while (menuRunning) {
        while (SDL_PollEvent(&menuEvent) != 0) {
            if (menuEvent.type == SDL_QUIT) {
                menuRunning = false;
                running = false;
                quit = false;
            } else if (menuEvent.type == SDL_KEYDOWN) {
                switch (menuEvent.key.keysym.sym) {
                    case SDLK_1:
                        // Start Game
                        menuRunning = false;
                        break;
                    case SDLK_2:
                        // Exit
                        menuRunning = false;
                        running = false;
                        quit = false;
                        break;
                    case SDLK_3:
                        std::cout << "This game is created in C++ and SDL2." << std::endl;
                        menuRunning = false;
                        running = false;
                        quit = false;
                        break;
                }
            }
        }
    }

    // Clean up resources for SDL_image, and the menu
    SDL_DestroyRenderer(menuRenderer);
    SDL_DestroyWindow(menuWindow);
    TTF_Quit();
    SDL_Quit();
}

void initializeGame() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load textures for snake head in different directions
    headRightTexture = IMG_LoadTexture(renderer, "images/fh_r.jpg");
    headUpTexture = IMG_LoadTexture(renderer, "images/fh_u.jpg");
    headLeftTexture = IMG_LoadTexture(renderer, "images/fh_l.jpg");
    headDownTexture = IMG_LoadTexture(renderer, "images/fh_d.jpg");

    // Load textures for snake body in different directions
    bodyRightTexture = IMG_LoadTexture(renderer, "images/2l.jpg");
    bodyUpTexture = IMG_LoadTexture(renderer, "images/2l_u.jpg");
    bodyLeftTexture = IMG_LoadTexture(renderer, "images/2l.jpg");
    bodyDownTexture = IMG_LoadTexture(renderer, "images/2l_u.jpg");

    // Load textures for snake body in different directions
    tailRightTexture = IMG_LoadTexture(renderer, "images/ft_r.jpg");
    tailUpTexture = IMG_LoadTexture(renderer, "images/ft_u.png");
    tailLeftTexture = IMG_LoadTexture(renderer, "images/ft_l.png");
    tailDownTexture = IMG_LoadTexture(renderer, "images/ft_d.png");
    

    SDL_Surface* foodSurface = IMG_Load("images/Cherry.png");
    foodTexture = SDL_CreateTextureFromSurface(renderer, foodSurface);
    SDL_FreeSurface(foodSurface);

    SDL_Surface* bmpSurface = SDL_LoadBMP("images/obst.bmp");
    bmpTexture = SDL_CreateTextureFromSurface(renderer, bmpSurface);
    SDL_FreeSurface(bmpSurface);

    font = TTF_OpenFont("fonts/airstrikebold.ttf", 24);

    collideEffect = Mix_LoadWAV("sound/try.WAV");
    laserEffect = Mix_LoadWAV("sound/mus.WAV");

    snakeX[0] = SCREEN_WIDTH / 2;
    snakeY[0] = SCREEN_HEIGHT / 2;

    foodX = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    foodY = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
}

void handleInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    direction = 1;
                    break;
                case SDLK_DOWN:
                    direction = 3;
                    break;
                case SDLK_LEFT:
                    direction = 2;
                    break;
                case SDLK_RIGHT:
                    direction = 0;
                    break;
            }
        }
    }
}


void updateGame() {
    for (int i = snakeLength; i > 0; --i) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    switch (direction) {
        case 0:
            snakeX[0] += GRID_SIZE;
            break;
        case 1:
            snakeY[0] -= GRID_SIZE;
            break;
        case 2:
            snakeX[0] -= GRID_SIZE;
            break;
        case 3:
            snakeY[0] += GRID_SIZE;
            break;
    }

    // Collision with obstacle BMP images
    SDL_Rect headRect = {snakeX[0], snakeY[0], GRID_SIZE, GRID_SIZE};
    SDL_Rect bmpRect1 = {0, 50, 700, 13};
    SDL_Rect bmpRect2 = {750, 40, 7, 300};
    SDL_Rect bmpRect3 = {20, 350, 11, SCREEN_HEIGHT/2};
    SDL_Rect bmpRect4 = {380, 480, SCREEN_WIDTH /2,14};

    if (SDL_HasIntersection(&headRect, &bmpRect1) ||
        SDL_HasIntersection(&headRect, &bmpRect2) ||
        SDL_HasIntersection(&headRect, &bmpRect3) ||
        SDL_HasIntersection(&headRect, &bmpRect4)) {
        Mix_PlayChannel(-1, laserEffect, 0);
        running = false;
    }


    // Check if the snake collides with itself
    for (int i = 1; i < snakeLength; ++i) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            Mix_PlayChannel(-1, laserEffect, 0);
            running = false;
        }
    }

    // Check if the snake eats the food
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        Mix_PlayChannel(-1, collideEffect, 0);
        snakeLength++;
        score += 10;

       //food generation:
        if (snakeX[0] == foodX && snakeY[0] == foodY) {
        Mix_PlayChannel(-1, collideEffect, 0);
        foodX = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        foodY = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    }
    }

    // Check if the snake goes out of bounds
    if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
        Mix_PlayChannel(-1, laserEffect, 0);
        running = false;
    }
}

void renderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* headDirectionTexture = nullptr;
    switch (direction) {
        case 0:
            headDirectionTexture = headRightTexture;
            break;
        case 1:
            headDirectionTexture = headUpTexture;
            break;
        case 2:
            headDirectionTexture = headLeftTexture;
            break;
        case 3:
            headDirectionTexture = headDownTexture;
            break;
    }

    SDL_Rect headRect = {snakeX[0], snakeY[0], GRID_SIZE, GRID_SIZE};
    SDL_RenderCopy(renderer, headDirectionTexture, nullptr, &headRect);

    for (int i = 1; i < snakeLength - 1; ++i) {
        SDL_Rect bodyRect = {snakeX[i], snakeY[i], GRID_SIZE, GRID_SIZE};

        SDL_Texture* bodyDirectionTexture = nullptr;
        switch (direction) {
            case 0:
                bodyDirectionTexture = bodyRightTexture;
                break;
            case 1:
                bodyDirectionTexture = bodyUpTexture;
                break;
            case 2:
                bodyDirectionTexture = bodyLeftTexture;
                break;
            case 3:
                bodyDirectionTexture = bodyDownTexture;
                break;
        }

        SDL_RenderCopy(renderer, bodyDirectionTexture, nullptr, &bodyRect);
    }

    if (snakeLength > 1) {
        SDL_Rect tailRect = {snakeX[snakeLength - 1], snakeY[snakeLength - 1], GRID_SIZE, GRID_SIZE};
        SDL_Texture* tailDirectionTexture=nullptr;
        switch(direction){
            case 0:
            tailDirectionTexture=tailRightTexture;
            break;
            case 1:
            tailDirectionTexture=tailUpTexture;
            break;
            case 2:
            tailDirectionTexture=tailLeftTexture;
            break;
            case 3:
            tailDirectionTexture=tailDownTexture;
            break;



        }
        SDL_RenderCopy(renderer, tailDirectionTexture, nullptr, &tailRect);
}

    SDL_Rect foodRect = {foodX, foodY, GRID_SIZE, GRID_SIZE};
    SDL_RenderCopy(renderer, foodTexture, nullptr, &foodRect);

    SDL_Rect bmpRect1 = {0, 50, 700, 13};
    SDL_Rect bmpRect2 = {750, 40, 7, 300};
    SDL_Rect bmpRect3 = {20, 350, 11, SCREEN_HEIGHT/2};
    SDL_Rect bmpRect4 = {380, 480, SCREEN_WIDTH /2,14};
    SDL_RenderCopy(renderer, bmpTexture, nullptr, &bmpRect1);
    SDL_RenderCopy(renderer, bmpTexture, nullptr, &bmpRect2);
    SDL_RenderCopy(renderer, bmpTexture, nullptr, &bmpRect3);
    SDL_RenderCopy(renderer, bmpTexture, nullptr, &bmpRect4);

    SDL_Color textColor = {255, 0, 0, 255};
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect scoreRect = {10, 10, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &scoreRect);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);
}

void closeGame() {
    SDL_DestroyTexture(headRightTexture);
    SDL_DestroyTexture(headUpTexture);
    SDL_DestroyTexture(headLeftTexture);
    SDL_DestroyTexture(headDownTexture);
    SDL_DestroyTexture(bodyRightTexture);
    SDL_DestroyTexture(bodyUpTexture);
    SDL_DestroyTexture(bodyLeftTexture);
    SDL_DestroyTexture(bodyDownTexture);
    SDL_DestroyTexture(tailRightTexture);
    SDL_DestroyTexture(tailUpTexture);
    SDL_DestroyTexture(tailLeftTexture);
    SDL_DestroyTexture(tailDownTexture);
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(bmpTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeChunk(collideEffect);
    Mix_FreeChunk(laserEffect);
    Mix_CloseAudio();
    SDL_Quit();
    TTF_Quit();
    IMG_Quit();
}


void renderFinalScore() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (TTF_Init() != 0) {
        std::cout << "TTF Initialization Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow("FINAL SCORE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load background texture
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "images/fs.jpg");
    if (!backgroundTexture) {
        std::cout << "Background Loading Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    // Render the background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_DestroyTexture(backgroundTexture);

    // Load a font
    TTF_Font* font = TTF_OpenFont("fonts/Roman SD.ttf", 40);
    if (!font) {
        std::cout << "Font Loading Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    // Text content and positions
    std::string textLines[] = {"Game Over", "Final Score: " + std::to_string(score), "Press Escape to Leave"};
    int lineHeight = 85;
    int yStartPos = 100;

    // Render and display each line of text with justified alignment
    for (const auto& line : textLines) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, line.c_str(), {255, 0, 0});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Print the final score in a different color
        if (line.find("Final Score") != std::string::npos) {
            SDL_FreeSurface(surface);
            surface = TTF_RenderText_Solid(font, line.c_str(), {253, 208, 23});
            SDL_DestroyTexture(texture);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
        }

        int xStartPos = (800 - surface->w) / 2; 
        SDL_Rect destRect = {xStartPos, yStartPos, surface->w, surface->h};
        yStartPos += lineHeight;

        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    // Present the renderer
    SDL_RenderPresent(renderer);

    
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

int main() {
    renderMenu();
    initializeGame();

    while (running) {
        handleInput();
        updateGame();
        renderGame();
        SDL_Delay(200);
    }

    if (!running) {
        closeGame();
        renderFinalScore();
    }

    return 0;
}
