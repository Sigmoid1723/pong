#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <string>

#define WIDTH 720
#define HEIGHT 720
#define FONT_SIZE 32
#define SIZE 16
#define BALL_SPEED 16
#define SPEED 9
#define PI 3.14159265358979323846

#define ONE_PLAYER // comment this for two player 

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font *font;
SDL_Color color;
bool running;
int frameCount, timerFPS, lastFrame, fps;

SDL_Rect l_paddle, r_paddle, ball, score_board;
float VelX, VelY;
std::string score;
int l_s, r_s; // score
bool turn;

void serve() {
  l_paddle.y = r_paddle.y = (HEIGHT/2) - (l_paddle.h/2);

  if(turn) {
    ball.x = l_paddle.x + (l_paddle.w*4);
    VelX = BALL_SPEED / 2;
  } else {
    ball.x = r_paddle.x - (r_paddle.w*4);
    VelX = -BALL_SPEED / 2;
  }
  VelY = 0;
  ball.y = HEIGHT/2 - (SIZE/2);
  turn = !turn;
}

void write(std::string text, int x, int y) {
  SDL_Surface *surface;
  SDL_Texture *texture;
  const char* t = text.c_str();
  surface = TTF_RenderText_Solid(font, t, color);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  score_board.w = surface->w;
  score_board.h = surface->h;
  score_board.x = x - score_board.w;
  score_board.y = y - score_board.h;

  SDL_FreeSurface(surface);
  SDL_RenderCopy(renderer, texture, NULL, &score_board);
  SDL_DestroyTexture(texture);
}

void update() {
  if(SDL_HasIntersection(&ball, &r_paddle)) {
    double rel = (r_paddle.y+(r_paddle.h/2))-(ball.y+(SIZE/2));
    double norm = rel/(r_paddle.h/2);
    double bounce = norm * (5*PI/12);
    VelX=-BALL_SPEED*cos(bounce);
    VelY=BALL_SPEED*-sin(bounce);
  }
  if(SDL_HasIntersection(&ball, &l_paddle)) {
    double rel = (l_paddle.y+(l_paddle.h/2))-(ball.y+(SIZE/2));
    double norm = rel/(l_paddle.h/2);
    double bounce = norm * (5*PI/12);
    VelX=BALL_SPEED*cos(bounce);
    VelY=BALL_SPEED*-sin(bounce);
  }
 
#ifdef ONE_PLAYER // if one player game then move r_paddle automatically based on ball.
  if(ball.y > r_paddle.y+(r_paddle.h/2)) r_paddle.y+=SPEED;
  if(ball.y < r_paddle.y+(r_paddle.h/2)) r_paddle.y-=SPEED;
#endif
 
  if(ball.x<=0) {r_s++;serve();}
  if(ball.x+SIZE>=WIDTH) {l_s++;serve();}
  if(ball.y<=0 || ball.y+SIZE>=HEIGHT) VelY=-VelY;
  ball.x+=VelX;
  ball.y+=VelY;
  score=std::to_string(l_s) + "   " + std::to_string(r_s);
  if(l_paddle.y<0)l_paddle.y=0;
  if(l_paddle.y+l_paddle.h>HEIGHT)l_paddle.y=HEIGHT-l_paddle.h;
  if(r_paddle.y<0)r_paddle.y=0;
  if(r_paddle.y+r_paddle.h>HEIGHT)r_paddle.y=HEIGHT-r_paddle.h;
}

void input() {
  SDL_Event e;
  const Uint8 *keystates = SDL_GetKeyboardState(NULL);
  while(SDL_PollEvent(&e)) if(e.type==SDL_QUIT) running = false;
  if(keystates[SDL_SCANCODE_ESCAPE]) running = false;
  
#ifdef ONE_PLAYER // two player then controll r_paddle with up down
  if(keystates[SDL_SCANCODE_UP]) l_paddle.y -= SPEED;
  if(keystates[SDL_SCANCODE_DOWN]) l_paddle.y += SPEED;
#else  
  if(keystates[SDL_SCANCODE_W]) l_paddle.y -= SPEED;
  if(keystates[SDL_SCANCODE_S]) l_paddle.y += SPEED;
  if(keystates[SDL_SCANCODE_UP]) r_paddle.y -= SPEED;
  if(keystates[SDL_SCANCODE_DOWN]) r_paddle.y += SPEED;
#endif
    
  
}

void render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  
  frameCount++;
  timerFPS = SDL_GetTicks() - lastFrame;
  if(timerFPS < (1000/60)) {
    SDL_Delay((1000/60) - timerFPS);
  }

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
  SDL_RenderFillRect(renderer, &l_paddle);
  SDL_RenderFillRect(renderer, &r_paddle);
  SDL_RenderFillRect(renderer, &ball);
  write(score, WIDTH/2+FONT_SIZE, FONT_SIZE*2);

  SDL_RenderPresent(renderer);
}

int main()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
  if(SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,0,&window,&renderer) < 0) std::cout << "Failed at SDL_CreateWindow&Renderer()" << std::endl;
  TTF_Init();
  font = TTF_OpenFont("FiraCodeNerdFont-Bold.ttf", FONT_SIZE);

  running = true;
  static int lastTime = 0;

  color.g = color.b = color.r = 255;
  l_s= r_s=0;
  l_paddle.x = 32; l_paddle.h = HEIGHT/4;
  l_paddle.y = (HEIGHT/2) - (l_paddle.h/2);
  l_paddle.w = 12;
  r_paddle = l_paddle;
  r_paddle.x = WIDTH - r_paddle.w - 32;
  ball.w = ball.h = SIZE;

  serve();

  while(running) {
    lastFrame = SDL_GetTicks();
    if(lastFrame>=(lastTime + 1000)) {
      lastTime = lastFrame;
      fps = frameCount;
      frameCount = 0;
    }

    update();
    input();
    render();
  }

  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
