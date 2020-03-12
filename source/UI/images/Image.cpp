#include <cstring>
#include "Image.hpp"
#include "stb_image.h"
#include "../../utils.hpp"

using namespace layoff::UI;

Image::~Image()
{
	if (!tex.pixels) return;
	if (stb)
		stbi_image_free(GetBuffer());
	else
		delete[] (u8*)tex.pixels;
}

namespace layoff::UI::images
{
	ImagePtr FromFile(const std::string& name)
	{
		auto img = std::make_unique<Image>(true);
		int channels = 0;

		img->SetBuffer(stbi_load(name.c_str(), &img->W(), &img->H(), &channels, 4));

		if (!img->GetBuffer())
		{
			PrintLn(name + " loading failed ! " + stbi_failure_reason());
		}

		return img;
	}

	ImagePtr FromBufferStbi(const u8* data, const u32 len)
	{
		auto img = std::make_unique<Image>(true);
		int channels = 0;

		img->SetBuffer(stbi_loadf_from_memory(data, len, &img->W(), &img->H(), &channels, 4));

		if (!img->GetBuffer())
		{
			PrintLn((std::string)"image loading failed ! " + stbi_failure_reason());
		}

		return img;
	}

	ImagePtr FromBuffer(const u8* data, u32 w, u32 h)
	{
		auto img = std::make_unique<Image>(false);

		img->W() = w;
		img->H() = h;
		img->SetBuffer(new u8[w * h * 4]);

		std::memcpy(img->GetBuffer(), data, w * h * 4);

		return img;
	}
}
