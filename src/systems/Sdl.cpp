#include "Sdl.hpp"
#include "Components.hpp"

sys::Sdl::Sdl() : pos_x(0), pos_y(0), win_w(640), win_h(480) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    throw Exception();
  window = SDL_CreateWindow("Patrician", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			    win_w, win_h, SDL_WINDOW_RESIZABLE);
  if (window == NULL)
    throw Exception();
  screen = SDL_GetWindowSurface(window);
  if (screen == NULL)
    throw Exception();
  sprites["city"] = SDL_CreateRGBSurface(0, 3, 3, 32, 0, 0, 0, 0);
  SDL_FillRect(sprites["city"], NULL, SDL_MapRGB(sprites["city"]->format, 255, 0, 0));
  sprites["ship"] = SDL_CreateRGBSurface(0, 3, 3, 32, 0, 0, 0, 0);
  SDL_FillRect(sprites["ship"], NULL, SDL_MapRGB(sprites["ship"]->format, 255, 255, 255));
}

sys::Sdl::~Sdl() {
  for (auto& it: sprites) {
    SDL_FreeSurface(it.second);
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void sys::Sdl::events(Ecs::World &world) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      world.stop();
    else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
    	pos_y -=10;
    	break;
      case SDLK_DOWN:
    	pos_y += 10;
    	break;
      case SDLK_RIGHT:
    	pos_x += 10;
    	break;
      case SDLK_LEFT:
    	pos_x -= 10;
      }
    }
    else if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
	  || event.window.event == SDL_WINDOWEVENT_RESIZED) {
	win_w = event.window.data1;
	win_h = event.window.data2;
	screen = SDL_GetWindowSurface(window);
	if (screen == NULL)
	  throw Exception();
      }
    }
  }
  if (pos_x > 1000 - win_w)
    pos_x = 1000 - win_w;
  if (pos_x < 0)
    pos_x = 0;
  if (pos_y > 1000 - win_h)
    pos_y = 1000 - win_h;
  if (pos_y < 0)
    pos_y = 0;
}

// Returns true if the surface has be drawn (within the window), false otherwise
bool sys::Sdl::blitSurface(SDL_Surface *s, Ecs::Entity *e) {
  SDL_Rect dst;
  comp::Position *pos = e->getComponent<comp::Position>();

  if (pos->x > pos_x && pos->y > pos_y
      && pos->x < pos_x + win_w
      && pos->y < pos_y + win_h) {
    dst.x = pos->x - pos_x;
    dst.y = pos->y - pos_y;
    SDL_BlitSurface(s, NULL, screen, &dst);
    return true;
  }
  return false;
}

void sys::Sdl::display(Ecs::World &world) {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  for (auto *it: world.getEntities()) {
    if (it->getComponent<comp::Type>()->type == Type::CITY) {
      blitSurface(sprites["city"], it);
    }
    else if (it->getComponent<comp::Type>()->type == Type::SHIP) {
      blitSurface(sprites["ship"], it);
    }
  }
  SDL_UpdateWindowSurface(window);
}

void sys::Sdl::update(Ecs::World &world) {
  events(world);
  display(world);
}
