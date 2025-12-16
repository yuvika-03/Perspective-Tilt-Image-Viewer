#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Texture* create_rotated_texture(SDL_Renderer* renderer, SDL_Texture* Viewer, int w, int h, double angle)
{
    SDL_Texture* target = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h
    );

    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_Rect dst;
    dst.x = 0;
    dst.y = 0;
    dst.w = w;
    dst.h = h;

    SDL_RenderCopyEx(renderer, Viewer, NULL, &dst, angle, NULL, SDL_FLIP_NONE);

    SDL_SetRenderTarget(renderer, NULL);
    return target;
}

void render_perspective_tilt(SDL_Renderer *renderer, SDL_Texture *texture, int img_w, int img_h, float zoom, float tilt, int win_w, int win_h)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    int slices = 300;
    float centerY = img_h / 2.0f;

    float totalHeight= img_h * zoom;
    int drawY = (win_h - totalHeight) / 2;

    for (int i = 0; i < slices; i++)
    {
        float t = (float)i / slices;
        float y = t * img_h;

        float depth = 1.0f + tilt * (y - centerY) / centerY;
        if (depth < 0.1f) depth = 0.1f;

        SDL_Rect Viewer;
        Viewer.x = 0;
        Viewer.y = (int)y;
        Viewer.w = img_w;
        Viewer.h = img_h / slices;

        SDL_Rect dst;
        dst.w = (int)(img_w * zoom * depth);
        dst.h = (int)(totalHeight / slices) + 1;
        dst.x = (win_w - dst.w) / 2;
        dst.y = drawY;

        drawY += dst.h;

        SDL_RenderCopy(renderer, texture, &Viewer, &dst);
    }

    SDL_RenderPresent(renderer);
}



int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const int width = 900;
    const int height = 600;
    const char *image_path = "assests/Bird.jpg";


    //initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //initialize sdl-image
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)))
    {
        printf("Image Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    //creating window
    SDL_Window *window = SDL_CreateWindow(
        "Image Viewer", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    // loading image
    SDL_Surface *surface = IMG_Load(image_path);
    if (!surface)
    {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    int img_w, img_h;
    SDL_QueryTexture(texture, NULL, NULL, &img_w, &img_h);

    float zoom = 1.0f;
    float tilt = 0.0f;
    double angle = 0.0;

    int running = 1;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;

                else if (event.key.keysym.sym == SDLK_UP)
                    tilt += 0.05f;

                else if (event.key.keysym.sym == SDLK_DOWN)
                    tilt -= 0.05f;

                else if (event.key.keysym.sym == SDLK_LEFT)
                    angle -= 5.0;

                else if (event.key.keysym.sym == SDLK_RIGHT)
                    angle += 5.0;

                else if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_PLUS)  // +
                    zoom += 0.1f;

                if (event.key.keysym.sym == SDLK_MINUS)   // -
                    zoom -= 0.1f;

                if (tilt > 1.0f) tilt = 1.0f;
                if (tilt < -1.0f) tilt = -1.0f;
                if (zoom < 0.1f) zoom = 0.1f;
            }
        }

        SDL_Texture* rotated = create_rotated_texture(renderer, texture, img_w, img_h, angle);

        render_perspective_tilt(renderer, rotated, img_w, img_h, zoom, tilt, width, height);

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}