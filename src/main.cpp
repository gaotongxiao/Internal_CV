#include <SDL2/SDL.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
using namespace std;

int main(int argc, char **argv)
{
  bool quit = false;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  SDL_Window *window = SDL_CreateWindow("My SDL Empty Window",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                                        SDL_WINDOW_FULLSCREEN_DESKTOP);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Surface * image[10];
  SDL_Texture * texture[10];
  int digit_width = DM.h / 130 * 86;
  SDL_Rect dstrect = { (DM.w - digit_width) / 2, 0, digit_width, DM.h };
  srand(time(NULL));
  for (int i = 0; i < 10; i++)
  {
    string t = "dig/";
    t += char(i + '0');
    t += ".bmp";
    image[i] = SDL_LoadBMP(t.c_str());
    texture[i] = SDL_CreateTextureFromSurface(renderer, image[i]);
  }

  int curDigit, lastDigit = -1;
  curDigit = rand() % 10;
  SDL_RenderCopy(renderer, texture[curDigit], NULL, &dstrect);
  SDL_RenderPresent(renderer);
  lastDigit = curDigit;

  time_t start_time = time(NULL);
  while (!quit)
  {
    if (time(NULL) - start_time)
    {
      curDigit = rand() % 10;
      SDL_RenderCopy(renderer, texture[curDigit], NULL, &dstrect);
      SDL_RenderPresent(renderer);
      lastDigit = curDigit;
      start_time = time(NULL);
    }
    while (SDL_PollEvent(&event))
      switch (event.type)
      {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              quit = true;
              break;
          }
          break;
      }
    }
  for (int i = 0; i < 10; i++)
  {
    SDL_DestroyTexture(texture[i]);
    SDL_FreeSurface(image[i]);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}