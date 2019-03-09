#pragma once
#include "UI/UI.hpp"
#include "UI/imgui_sdl.h"

class StatusDisplay
{
    public:
    StatusDisplay(){
        texTarget = new Texture();
		texTarget->Source = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 32);
		texTarget->Surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 32, 32, SDL_PIXELFORMAT_RGBA32);
    }
    ~StatusDisplay(){
        ImGuiSDL::FreeTexture(texTarget);
    }
    void Draw()
    {
        SDL_SetRenderTarget(sdl_render, texTarget->Source);
        SDL_SetRenderDrawColor(sdl_render, 255, 255, 255, 255);
        SDL_RenderDrawRect(sdl_render, &Battery);
        SDL_RenderDrawRect(sdl_render, &BatteryT);
        u32 percent;
        psmGetBatteryChargePercentage(&percent);
        SDL_Rect BatteryP = {2,2,static_cast<int>((percent/100)*28),14};
        SDL_RenderFillRect(sdl_render, &BatteryP);
        ImGui::Image(texTarget, ImVec2(512, 32));
    }
    private:
    Texture *texTarget;
    SDL_Rect Battery =  {0,0,32,16};
    SDL_Rect BatteryT = {32,4,4,8};
};