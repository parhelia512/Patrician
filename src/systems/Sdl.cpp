#include "Sdl.hpp"
#include "PlayerBuilder.hpp"

sys::Sdl::Sdl() : _win_w(640), _win_h(480), _velocity_x(0), _velocity_y(0), _player(0), _selected(NULL) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    throw Exception::Sdl();
  _window = SDL_CreateWindow("Patrician", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			     _win_w, _win_h, SDL_WINDOW_RESIZABLE);
  if (_window == NULL)
    throw Exception::Sdl();
  _screen = SDL_GetWindowSurface(_window);
  if (_screen == NULL)
    throw Exception::Sdl();
  _hud = new Gui::Hud(640, 480);
  _game = new Gui::Game(640, 480 - 20);
  _popin = new Gui::Popin(300, 300);
}

sys::Sdl::~Sdl() {
  delete _hud;
  delete _game;
  delete _popin;
  SDL_DestroyWindow(_window);
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
    	_velocity_y = -3;
    	break;
      case SDLK_DOWN:
    	_velocity_y = 3;
    	break;
      case SDLK_RIGHT:
    	_velocity_x = 3;
    	break;
      case SDLK_LEFT:
    	_velocity_x = -3;
      }
    }
    else if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
      case SDLK_DOWN:
    	_velocity_y = 0;
    	break;
      case SDLK_RIGHT:
      case SDLK_LEFT:
    	_velocity_x = 0;
      }
    }
    else if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
	  || event.window.event == SDL_WINDOWEVENT_RESIZED) {
	_win_w = event.window.data1;
	_win_h = event.window.data2;
	_hud->updateSize(_win_w, _win_h);
	_game->updateSize(_win_w, _win_h - 20);
	_screen = SDL_GetWindowSurface(_window);
	if (_screen == NULL)
	  throw Exception::Sdl();
      }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
      if (_selected
	  && event.button.button == SDL_BUTTON_LEFT
	  && (uint32_t)event.button.x > (_screen->w - _popin->getW()) / 2
	  && (uint32_t)event.button.x < (_screen->w + _popin->getW()) / 2
	  && (uint32_t)event.button.y > (_screen->h - _popin->getW()) / 2
	  && (uint32_t)event.button.y < (_screen->h + _popin->getW()) / 2) {
	_popin->click(event.button.x - (_screen->w - _popin->getW()) / 2,
		       event.button.y - (_screen->h - _popin->getH()) / 2);
      }
      else if (event.button.button == SDL_BUTTON_LEFT
	       && event.button.y >= 20) {
	_selected = _game->click(event.button.x, event.button.y - 20);
      }
    }
  }
  _game->updatePos(_velocity_x, _velocity_y);
}

void sys::Sdl::display(Ecs::World &world) {
  SDL_FillRect(_screen, NULL, SDL_MapRGB(_screen->format, 0, 0, 0));
  SDL_BlitSurface(_hud->draw(world, _player), NULL, _screen, NULL);

  SDL_Rect dst;
  dst.x = 0;
  dst.y = 20;
  SDL_BlitSurface(_game->draw(world, _player), NULL, _screen, &dst);
  if (_selected
      && _selected->get<comp::Type>()->type == Type::CITY) {
    SDL_Surface *popinSurface = _popin->draw(world, _selected);
    dst.x = (_screen->w - popinSurface->w) / 2;
    dst.y = (_screen->h - popinSurface->h) / 2;
    SDL_BlitSurface(popinSurface, NULL, _screen, &dst);
  }
  SDL_UpdateWindowSurface(_window);
}

void sys::Sdl::update(Ecs::World &world) {
  if (!_player)
    _player = *::players.begin();
  events(world);
  display(world);
}
