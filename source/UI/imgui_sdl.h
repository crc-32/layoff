#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>

struct ImDrawData;
struct SDL_Renderer;

struct ImVec2;
struct Color;

struct Texture
{
public:
	SDL_Surface* Surface;
	SDL_Texture* Source;

	~Texture();
	Color Sample(float u, float v) const;
	ImVec2 Size();
};

namespace ImGuiSDL
{
	// Call this to initialize the SDL renderer device that is internally used by the renderer.
	void Initialize(SDL_Renderer* renderer, int windowWidth, int windowHeight);
	// Call this before destroying your SDL renderer or ImGui to ensure that proper cleanup is done. This doesn't do anything critically important though,
	// so if you're fine with small memory leaks at the end of your application, you can even omit this.
	void Deinitialize();

	// Call this every frame after ImGui::Render with ImGui::GetDrawData(). This will use the SDL_Renderer provided to the interfrace with Initialize
	// to draw the contents of the draw data to the screen.
	void Render(ImDrawData* drawData);

	Texture* LoadTexture(const std::string &path);
	Texture* LoadTexture(const std::vector<unsigned char> &data);
	void FreeTexture(Texture* tex);
};