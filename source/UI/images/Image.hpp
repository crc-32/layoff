#pragma once
#include <imgui/imgui.h>
#include "../rendering/imgui_sw.hpp"
#include <memory>
#include <string>

namespace layoff::UI
{
	class Image 
	{
	public:
		~Image();
		Image(bool stb) : stb(stb) 
		{
			tex.format = imgui_sw::TextureFormat::RGBA;
		}

		ImTextureID GetID() { return &tex; }

		void* GetBuffer() { return (void*)tex.pixels; }
		void SetBuffer(void* buf)
		{
			tex.pixels = (const u8*)buf;
		}

		ImVec2 GetSize() const { return { (float)tex.width, (float)tex.height }; }

		int& W() { return tex.width; }
		int& H() { return tex.height; }

		bool IsStb() { return stb; }
	private:
		bool stb = false;
		imgui_sw::Texture tex;
	};

	using ImagePtr = std::unique_ptr<Image>;

	namespace images
	{
		ImagePtr FromFile(const std::string& name);
		ImagePtr FromBufferStbi(const u8* data, const u32 len);
		ImagePtr FromBuffer(const u8* data, u32 w, u32 h);
	}

}
