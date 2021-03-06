#pragma once

#include <cstdint>
#include <map>
#include <list>
#include <string>
#include <SDL.h>
#include "World.hpp"
#include "src/Exceptions.hpp"
#include "gui/Base.hpp"
#include "gui/TextRender.hpp"

namespace Gui {
  class Game {
  private:
    uint32_t _width;
    uint32_t _height;
    uint32_t _pos_x;
    uint32_t _pos_y;
    SDL_Surface *_surface;
    std::list<std::pair<SDL_Rect, Ecs::Entity *>> _blittedEntities;
    std::map<std::string, SDL_Surface *> _sprites;

  public:
    Game(uint32_t width, uint32_t height);
    ~Game();
    void updateSize(uint32_t width, uint32_t height);
    void updatePos(int32_t x, int32_t y);
    SDL_Surface *draw(Ecs::World &world, uint32_t player);
    Ecs::Entity *click(int32_t x, int32_t y);

  private:
    bool blitSurface(SDL_Surface *surface, uint32_t x, uint32_t y);
    bool blitSurface(SDL_Surface *surface, Ecs::Entity *it);
    bool blitSurface(std::string const& name, uint32_t x, uint32_t y);
    bool blitSurface(std::string const& name, Ecs::Entity *it);
    void writeText(std::string const& content, int x, int y, Gui::align alignment = Gui::align::LEFT);
  };
}
