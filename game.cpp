using namespace std;
#include <SDL2/SDL.h>
#include <ctime>

namespace Breakout
{
   SDL_Window *window = NULL;
   SDL_Renderer *renderer = NULL;
   SDL_Rect ball;

   const int SCREEN_WIDTH = 680;
   const int SCREEN_HEIGHT = 680;
   const int STICK_OFFSET = 10;

   int ballDim[2] = {12, 12};
   int grid[400];
   int stickPositions[50];
   float velocity[2] = {.9, -.9};
   float ballPos[2];

   void gameLoop(bool changed = false, bool right = true);
   void maintainStick(bool changed = false, bool right = true);

   void createGrid(int rows = 20, int cols = 20)
   {
      SDL_Rect rect;

      int count = 0;
      for (int y = 1; y <= rows; y++)
      {
         for (int x = 1; x <= cols; x++)
         {
            if (grid[count] > 0)
            {
               rect.x = (SCREEN_WIDTH / rows) * (x - 1);
               rect.y = ((y - 1) * ((SCREEN_HEIGHT / 2) / cols));
               rect.w = (SCREEN_WIDTH / rows);
               rect.h = (SCREEN_HEIGHT / 2) / (cols);
               if (grid[count] == 1)
               {
                  SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
               }
               else
               {
                  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
               }
               SDL_RenderFillRect(renderer, &rect);
            }
            count++;
         }
      }
   }

   void checkCollison(int rows = 20, int cols = 20)
   {
      // Checking collison with the sides of the window
      ballPos[0] += velocity[0];
      if (ballPos[0] < 0 || ballPos[0] + ball.w > SCREEN_WIDTH)
      {
         velocity[0] = -velocity[0];
         ballPos[0] += velocity[0] * 2;
      }
      ball.x = (int)(ballPos[0]);

      ballPos[1] += velocity[1];
      if (ballPos[1] < 0 || ballPos[1] + ball.h > SCREEN_HEIGHT)
      {
         velocity[1] = -velocity[1];
         ballPos[1] += velocity[1] * 2;
      }
      ball.y = (int)(ballPos[1]);

      // Check collison with  grid
      int count = 0;
      for (int y = 1; y <= rows; y++)
      {
         for (int x = 1; x <= cols; x++)
         {
            if (grid[count] > 0)
            {
               int xpos = (SCREEN_WIDTH / rows) * (x - 1);
               int ypos = ((y - 1) * ((SCREEN_HEIGHT / 2) / cols));
               int xw = (SCREEN_WIDTH / rows);
               int yh = (SCREEN_HEIGHT / 3) / (cols * 3);
               if (ball.x >= xpos && ball.x <= xpos + xw && ball.y >= ypos && ball.y <= ypos + yh)
               {
                  velocity[0] = -velocity[0];
                  velocity[1] = -velocity[1];
                  ballPos[0] += velocity[0];
                  ballPos[1] += velocity[1];
                  ball.x = (int)(ballPos[0]);
                  ball.y = (int)(ballPos[1]);

                  grid[count] = grid[count] - 1;
                  return;
               }
            }
            count++;
         }
      }

      // Check collison with stick
      int pos = 1;
      int row = sizeof(stickPositions) / sizeof(int);
      for (int i = 0; i < row; i++)
      {
         if (stickPositions[i] == 1)
         {
            pos = i;
            break;
         }
      }
      int xpos = ((SCREEN_WIDTH / row) * pos);
      int ypos = (SCREEN_HEIGHT - (STICK_OFFSET + 20));
      int xw = (SCREEN_WIDTH / 10);
      int yh = (SCREEN_HEIGHT / 3) / (10 * 3);
      if (ball.x >= xpos && ball.x <= xpos + xw && ball.y >= ypos && ball.y <= ypos + yh)
      {
         velocity[1] = -velocity[1];
         return;
      }

      // Render ball
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderFillRect(renderer, &ball);
   }

   void maintainStick(bool changed, bool right)
   {
      SDL_Rect rect;
      int pos = 1;
      int rows = sizeof(stickPositions) / sizeof(int);

      for (int i = 0; i < rows; i++)
      {
         if (stickPositions[i] == 1)
         {
            pos = i;
            break;
         }
      }
      if (changed)
      {
         if (!right)
         {
            if (pos - 2 >= 0)
            {
               stickPositions[pos] = 0;
               stickPositions[pos - 2] = 1;
               pos -= 2;
            }
            else if ((pos - 1 >= 0))
            {
               stickPositions[pos] = 0;
               stickPositions[pos - 1] = 1;
               pos -= 1;
            }
         }
         else
         {
            if (pos + 2 < rows)
            {
               stickPositions[pos] = 0;
               stickPositions[pos + 2] = 1;
               pos += 2;
            }
            if (pos + 1 < rows)
            {
               stickPositions[pos] = 0;
               stickPositions[pos + 1] = 1;
               pos += 1;
            }
         }
      }

      rect.x = ((SCREEN_WIDTH / rows) * pos);
      rect.y = (SCREEN_HEIGHT - (STICK_OFFSET + 20));
      rect.w = (SCREEN_WIDTH / 10);
      rect.h = (SCREEN_HEIGHT / 3) / (10 * 3);

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderFillRect(renderer, &rect);
   }

   void gameLoop(bool changed, bool right)
   {
      SDL_RenderClear(renderer);

      SDL_Rect rect;
      rect.x = 0;
      rect.y = 0;
      rect.w = SCREEN_WIDTH;
      rect.h = SCREEN_HEIGHT;

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderFillRect(renderer, &rect);

      maintainStick(changed, right);
      checkCollison();
      createGrid();

      SDL_RenderPresent(renderer);
   }

   void driver()
   {
      // Initialize SDL
      SDL_Init(SDL_INIT_VIDEO);
      // Create window
      window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

      // Fill array
      srand(time(NULL));

      int max = 2;
      int min = 1;
      for (int i = 0; i < sizeof(grid) / sizeof(int); i++)
         grid[i] = 1 + (rand() % 2);

      int middle = (sizeof(stickPositions) / sizeof(int)) / 2;
      for (int i = 0; i < sizeof(stickPositions) / sizeof(int); i++)
      {
         if (i != middle - 1)
            stickPositions[i] = 0;
         else
            stickPositions[i] = 1;
      }

      // create dot
      int rows = (sizeof(stickPositions) / sizeof(int));

      ball.x = (SCREEN_WIDTH / 2);
      ball.y = (SCREEN_HEIGHT - (STICK_OFFSET + 21 + ballDim[1]));
      ball.h = ballDim[1];
      ball.w = ballDim[0];
      ballPos[0] = ball.x;
      ballPos[1] = ball.y;

      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (quit == false)
      {
         while (SDL_PollEvent(&e))
         {
            if (e.type == SDL_QUIT)
            {
               quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
               if (e.key.keysym.sym == SDLK_a)
                  gameLoop(true, false);
               else if (e.key.keysym.sym == SDLK_d)
                  gameLoop(true);
            }
         }
         gameLoop();
      }

      SDL_DestroyWindow(window);
      SDL_Quit();
   }
}

int main(int argc, char *args[])
{
   Breakout::driver();
   return 0;
}
