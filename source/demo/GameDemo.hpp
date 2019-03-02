#pragma once
#include <iostream>
#include <math.h>
#include "../UI/UI.hpp"

class DemoGame : public UiItem 
{
public:
	bool Draw() override 
	{
		if (!Running) return false;
		if (!ImGui::Begin("T-Rex", &Running, ImGuiWindowFlags_NoResize))
		{
			ImGui::End();
			return Running;
		}
		ImGui::SetWindowSize(ImVec2(415, 260));
		if (GameState == 0)
		{
			loaderCounter++;
			ImGui::Text("Tired of waiting during loading\nscreens ?"); 
			if (loaderCounter > 60)
				ImGui::Text("Then watch this !");
			if (loaderCounter > 120)
				GameState = 1;
		}
		else
		{
			SDL_SetRenderTarget(sdl_render, texTarget->Source);
			SDL_SetRenderDrawColor(sdl_render, 247, 247, 247, 255);
			SDL_RenderClear(sdl_render);
			if (GameState == 1) Game();
			else GameOverScreen();
			SDL_SetRenderTarget(sdl_render, NULL);
			ImGui::Image(texTarget, ImVec2(400, 200));
		}
		ImGui::End();
		return true; //can't return false here as texTarget would be deallocated but ImGui would still try to render it at the end of the frame.
	}

	DemoGame() : io(ImGui::GetIO())
	{
		Sprites = ImGuiSDL::LoadTexture("romfs:/demo/DinoSprites.png");
		texTarget = new Texture();
		texTarget->Source = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 200);
		texTarget->Surface = SDL_CreateRGBSurfaceWithFormat(0, 400, 200, 32, SDL_PIXELFORMAT_RGBA32);
		srand(time(NULL));
	}

	~DemoGame() 
	{
		ImGuiSDL::FreeTexture(Sprites);
		ImGuiSDL::FreeTexture(texTarget);
	}

private:
	int loaderCounter = 0;
	ImGuiIO &io;
	int GameState = 0; //0: intro, 1: game, 2: game over

	bool Running = true;
	Texture *Sprites;
	const SDL_Rect PLAYER = {765 ,2, 44,48};
	const SDL_Rect OBSTACLES[3] = { { 332, 2, 25, 50 }, { 432, 2, 50, 50 }, { 228,2 ,18,35 } };
	const SDL_Rect CLOUD = { 86, 2, 46,13 };
	const SDL_Rect GROUND = { 2, 54, 1200, 14 };
	const SDL_Rect GAMEOVER = { 484, 15, 191, 11 };
	
	Texture *texTarget;

	int GroundX = 0;
	int GameSpeed = 10;

	const int PlayerBaseY = 200 - 10 - PLAYER.h;
	int PlayerY = PlayerBaseY;
	int PlayerYSpeed = 0;

	s64 lastPlayerFrameSwitched = 0;
	SDL_Rect PlayerSrc = PLAYER;

	s64 lastSpawnFramesElapsed = 0;
	struct GameObj 
	{
		SDL_Rect const *rect;
		int X;
	};
	std::vector<GameObj> objs;

	int CloudX, CloudY;
	int CloudX2, CloudY2;

	void InitGame()
	{
		objs.clear();
		GroundX = 0;
		GameSpeed = 10;
		PlayerY = PlayerBaseY;
		PlayerYSpeed = 0;
		lastPlayerFrameSwitched = 0;
		PlayerSrc = PLAYER;
		lastSpawnFramesElapsed = 0;
		framesElapsed = 0;
		GameState = 1;
		CloudX = 400;
		CloudY = rand() % 120 + 10;
		CloudX2 = 561;
		CloudY2 = rand() % 100;
	}

	bool DoesCollideWithPlayer(const SDL_Rect &r) {
		//SDL_SetRenderDrawColor(sdl_render, 255, 0, 0, 255);
		SDL_Rect en = r;
		en.h -= 14;	en.y += 14;	en.x += 4;	en.w -= 8;
		//SDL_RenderFillRect(sdl_render, &en);
		SDL_Rect pl = { 60 + 8, PlayerY + 10, PLAYER.w - 16, PLAYER.h -20 };
		//SDL_RenderFillRect(sdl_render, &pl);

		return (pl.x + pl.w >= en.x && en.x + en.w >= pl.x) && (pl.y + pl.h >= en.y && en.y + en.h >= pl.y);
	}

	s64 framesElapsed = 0;
	void Game() 
	{
		framesElapsed++;		
		//Ground
		GroundX -= GameSpeed;
		if (GroundX + GROUND.w <= 0) GroundX = 0;
		SDL_Rect target = { GroundX , 200 - 20, GROUND.w , GROUND.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &GROUND, &target); 
		if (GroundX + GROUND.w < 400)
		{
			target.x = GroundX + GROUND.w;
			SDL_RenderCopy(sdl_render, Sprites->Source, &GROUND, &target);
		}
		//Draw cloud
		CloudX -= GameSpeed / 3;
		CloudX2 -= GameSpeed / 4;
		target = { CloudX,CloudY, CLOUD.w, CLOUD.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &CLOUD, &target);
		target = { CloudX2,CloudY2, CLOUD.w, CLOUD.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &CLOUD, &target);
		if (CloudX < -CLOUD.w)
		{
			CloudX = 400;
			CloudY = rand() % 120 + 10;
		}
		if (CloudX2 < -CLOUD.w)
		{
			CloudX2 = 561;
			CloudY2 = rand() % 100;
		}
		//Calculate player physics, needs to be done before collisions
		if ((io.MouseClicked[0] || (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A)) && PlayerYSpeed == 0) PlayerYSpeed = -17;
		if (PlayerYSpeed != 0)
		{
			PlayerY += PlayerYSpeed;
			PlayerYSpeed += 2;
			if (PlayerY >= PlayerBaseY) { PlayerYSpeed = 0; PlayerY = PlayerBaseY; }
			else if (PlayerYSpeed == 0) PlayerYSpeed = 1;
		}
		//Obstacles
		if (framesElapsed - lastSpawnFramesElapsed >= 41 - GameSpeed + ((rand() % 60) - 10)) 
		{
			objs.push_back(GameObj{ OBSTACLES + (rand() % 3), 400 });
			lastSpawnFramesElapsed = framesElapsed;
		}
		for (int i = 0; i < objs.size(); i++)
		{
			objs[i].X -= GameSpeed;
			if (objs[i].X <= -objs[i].rect->w)
				objs.erase(objs.begin() + (i--));
			else
			{
				target = *objs[i].rect;
				target.y = 200 - 10 - target.h;
				target.x = objs[i].X;
				SDL_RenderCopy(sdl_render, Sprites->Source, objs[i].rect, &target);

				if (DoesCollideWithPlayer(target))
				{
					PlayerSrc.x = PLAYER.x + PLAYER.w * 2;
					GameState = 2;
				}
			}
		}
		//Draw Player
		if ((framesElapsed - lastPlayerFrameSwitched >= max(3 ,15 - GameSpeed)) && PlayerYSpeed == 0 && GameState == 1)
		{
			lastPlayerFrameSwitched = framesElapsed;
			PlayerSrc.x += (PlayerSrc.x == PLAYER.x) ? PLAYER.w : -PLAYER.w;
		}
		target = { 60, PlayerY , PLAYER.w, PLAYER.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &PlayerSrc, &target);

		if (framesElapsed > (15 + GameSpeed) * 30 && GameSpeed < 20)
			GameSpeed++;
	}

	int max(int a, int b) { return a > b ? a : b; }

	void GameOverScreen()
	{
		//Ground		
		if (GroundX + GROUND.w <= 0) GroundX = 0;
		SDL_Rect target = { GroundX , 200 - 20, GROUND.w , GROUND.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &GROUND, &target);
		if (GroundX + GROUND.w < 400)
		{
			target.x = GroundX + GROUND.w;
			SDL_RenderCopy(sdl_render, Sprites->Source, &GROUND, &target);
		}
		//Draw clouds
		target = { CloudX,CloudY, CLOUD.w, CLOUD.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &CLOUD, &target);
		target = { CloudX2,CloudY2, CLOUD.w, CLOUD.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &CLOUD, &target);
		//Obstacle
		for (int i = 0; i < objs.size(); i++)
		{
			target = *objs[i].rect;
			target.y = 200 - 10 - target.h;
			target.x = objs[i].X;
			SDL_RenderCopy(sdl_render, Sprites->Source, objs[i].rect, &target);
			if (DoesCollideWithPlayer(target));
		}

		target = { 60, PlayerY, PLAYER.w, PLAYER.h };
		SDL_RenderCopy(sdl_render, Sprites->Source, &PlayerSrc, &target);
		
		target.w = GAMEOVER.w; target.h = GAMEOVER.h;
		target.x = (400 - GAMEOVER.w) / 2; target.y = (200 - GAMEOVER.y) / 2;
		SDL_RenderCopy(sdl_render, Sprites->Source, &GAMEOVER, &target);
		
		if (io.MouseClicked[0] || io.NavInputs[ImGuiNavInput_Activate])
			InitGame();
	}
};