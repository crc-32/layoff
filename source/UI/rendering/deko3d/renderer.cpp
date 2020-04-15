#include "renderer.h"

#include "imgui_deko3d.h"
#include "imgui_nx.h"

#include <imgui.h>

#include <zstd.h>

#include <switch.h>

#include <sys/stat.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <numeric>
#include <thread>

/// \brief Texture index
enum TextureIndex
{
	DEKO3D_LOGO = 1,
	BATTERY_ICON,
	CHARGING_ICON,
	ETH_NONE_ICON,
	ETH_ICON,
	AIRPLANE_ICON,
	WIFI_NONE_ICON,
	WIFI1_ICON,
	WIFI2_ICON,
	WIFI3_ICON,

	MAX_TEXTURE,
};

/*
/// \brief deko3d logo width
constexpr auto LOGO_WIDTH = 500;
/// \brief deko3d logo height
constexpr auto LOGO_HEIGHT = 493;

/// \brief icon width
constexpr auto ICON_WIDTH = 24;
/// \brief icon height
constexpr auto ICON_HEIGHT = 24;
*/

/// \brief Maximum number of samplers
constexpr auto MAX_SAMPLERS = 2;
/// \brief Maximum number of images
constexpr auto MAX_IMAGES = MAX_TEXTURE;

/// \brief Number of framebuffers
constexpr auto FB_NUM = 2u;

/// \brief Command buffer size
constexpr auto CMDBUF_SIZE = 1024 * 1024;

/// \brief Framebuffer width
unsigned s_width = 1280;
/// \brief Framebuffer height
unsigned s_height = 720;

/// \brief deko3d device
dk::UniqueDevice s_device;

/// \brief Depth buffer memblock
dk::UniqueMemBlock s_depthMemBlock;
/// \brief Depth buffer image
dk::Image s_depthBuffer;

/// \brief Framebuffer memblock
dk::UniqueMemBlock s_fbMemBlock;
/// \brief Framebuffer images
dk::Image s_frameBuffers[FB_NUM];

/// \brief Command buffer memblock
dk::UniqueMemBlock s_cmdMemBlock[FB_NUM];
/// \brief Command buffers
dk::UniqueCmdBuf s_cmdBuf[FB_NUM];

/// \brief Image memblock
dk::UniqueMemBlock s_imageMemBlock;

/// \brief Image/Sampler descriptor memblock
dk::UniqueMemBlock s_descriptorMemBlock;
/// \brief Sample descriptors
dk::SamplerDescriptor *s_samplerDescriptors = nullptr;
/// \brief Image descriptors
dk::ImageDescriptor *s_imageDescriptors = nullptr;

/// \brief deko3d queue
dk::UniqueQueue s_queue;

/// \brief deko3d swapchain
dk::UniqueSwapchain s_swapchain;

/// \brief Rebuild swapchain
/// \param width_ Framebuffer width
/// \param height_ Framebuffer height
/// \note This assumes the first call is the largest a framebuffer will ever be
void rebuildSwapchain (unsigned const width_, unsigned const height_)
{
	// destroy old swapchain
	s_swapchain = nullptr;

	// create new depth buffer image layout
	dk::ImageLayout depthLayout;
	dk::ImageLayoutMaker{s_device}
	    .setFlags (DkImageFlags_UsageRender | DkImageFlags_HwCompression)
	    .setFormat (DkImageFormat_Z24S8)
	    .setDimensions (width_, height_)
	    .initialize (depthLayout);

	auto const depthAlign = depthLayout.getAlignment ();
	auto const depthSize  = depthLayout.getSize ();

	// create depth buffer memblock
	if (!s_depthMemBlock)
	{
		s_depthMemBlock = dk::MemBlockMaker{s_device,
		    imgui::deko3d::align (
		        depthSize, std::max<unsigned> (depthAlign, DK_MEMBLOCK_ALIGNMENT))}
		                      .setFlags (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
		                      .create ();
	}

	s_depthBuffer.initialize (depthLayout, s_depthMemBlock, 0);

	// create framebuffer image layout
	dk::ImageLayout fbLayout;
	dk::ImageLayoutMaker{s_device}
	    .setFlags (
	        DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
	    .setFormat (DkImageFormat_RGBA8_Unorm)
	    .setDimensions (width_, height_)
	    .initialize (fbLayout);

	auto const fbAlign = fbLayout.getAlignment ();
	auto const fbSize  = fbLayout.getSize ();

	// create framebuffer memblock
	if (!s_fbMemBlock)
	{
		s_fbMemBlock = dk::MemBlockMaker{s_device,
		    imgui::deko3d::align (
		        FB_NUM * fbSize, std::max<unsigned> (fbAlign, DK_MEMBLOCK_ALIGNMENT))}
		                   .setFlags (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
		                   .create ();
	}

	// initialize swapchain images
	std::array<DkImage const *, FB_NUM> swapchainImages;
	for (unsigned i = 0; i < FB_NUM; ++i)
	{
		swapchainImages[i] = &s_frameBuffers[i];
		s_frameBuffers[i].initialize (fbLayout, s_fbMemBlock, i * fbSize);
	}

	// create swapchain
	s_swapchain = dk::SwapchainMaker{s_device, nwindowGetDefault (), swapchainImages}.create ();
	dkSwapchainSetSwapInterval(s_swapchain, 2);
}

#include <iostream>
#include <fstream>

void DKError(void* userData, const char* context, DkResult result) {
	std::ofstream f;
	f.open("sdmc:/dk_error.txt");
	f << "ERR: " << context << " (result " << result << ")" << std::endl;
	f.flush();
	f.close();
	fatalThrow(MAKERESULT(133,7000));
}

/// \brief Initialize deko3d
void deko3dInit ()
{
	// create deko3d device
	s_device = dk::DeviceMaker{}.setCbError(DKError).create ();

	// initialize swapchain with maximum resolution
	rebuildSwapchain (1280, 720);

	// create memblocks for each image slot
	for (std::size_t i = 0; i < FB_NUM; ++i)
	{
		// create command buffer memblock
		s_cmdMemBlock[i] =
		    dk::MemBlockMaker{s_device, imgui::deko3d::align (CMDBUF_SIZE, DK_MEMBLOCK_ALIGNMENT)}
		        .setFlags (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
		        .create ();

		// create command buffer
		s_cmdBuf[i] = dk::CmdBufMaker{s_device}.create ();
		s_cmdBuf[i].addMemory (s_cmdMemBlock[i], 0, s_cmdMemBlock[i].getSize ());
	}

	// create image/sampler memblock
	static_assert (sizeof (dk::ImageDescriptor) == DK_IMAGE_DESCRIPTOR_ALIGNMENT);
	static_assert (sizeof (dk::SamplerDescriptor) == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
	static_assert (DK_IMAGE_DESCRIPTOR_ALIGNMENT == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
	s_descriptorMemBlock = dk::MemBlockMaker{s_device,
	    imgui::deko3d::align (
	        (MAX_SAMPLERS + MAX_IMAGES) * sizeof (dk::ImageDescriptor), DK_MEMBLOCK_ALIGNMENT)}
	                           .setFlags (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
	                           .create ();

	// get cpu address for descriptors
	s_samplerDescriptors =
	    static_cast<dk::SamplerDescriptor *> (s_descriptorMemBlock.getCpuAddr ());
	s_imageDescriptors =
	    reinterpret_cast<dk::ImageDescriptor *> (&s_samplerDescriptors[MAX_SAMPLERS]);

	// create queue
	s_queue = dk::QueueMaker{s_device}.setFlags (DkQueueFlags_Graphics).create ();

	auto &cmdBuf = s_cmdBuf[0];

	// bind image/sampler descriptors
	cmdBuf.bindSamplerDescriptorSet (s_descriptorMemBlock.getGpuAddr (), MAX_SAMPLERS);
	cmdBuf.bindImageDescriptorSet (
	    s_descriptorMemBlock.getGpuAddr () + MAX_SAMPLERS * sizeof (dk::SamplerDescriptor),
	    MAX_IMAGES);
	s_queue.submitCommands (cmdBuf.finishList ());
	s_queue.waitIdle ();

	cmdBuf.clear ();
}
//TODO
/// \brief Load textures
void loadTextures ()
{
	struct TextureInfo
	{
		TextureInfo (char const *const path_, unsigned const width_, unsigned const height_)
		    : path (path_), width (width_), height (height_)
		{
		}

		char const *const path;
		unsigned width;
		unsigned height;
	};

	/*TextureInfo textureInfos[] = {TextureInfo ("romfs:/deko3d.rgba.zst", LOGO_WIDTH, LOGO_HEIGHT),
	    TextureInfo ("romfs:/battery_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/charging_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/eth_none_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/eth_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/airplane_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/wifi_none_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/wifi1_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/wifi2_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT),
	    TextureInfo ("romfs:/wifi3_icon.rgba.zst", ICON_WIDTH, ICON_HEIGHT)};*/
	TextureInfo textureInfos[] = {};

	// create memblock for transfer (large enough for the largest source file)
	dk::UniqueMemBlock memBlock =
	    dk::MemBlockMaker{s_device, imgui::deko3d::align (1048576, DK_MEMBLOCK_ALIGNMENT)}
	        .setFlags (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
	        .create ();

	// create image memblock (large enough to hold all the images)
	s_imageMemBlock = dk::MemBlockMaker{s_device,
	    imgui::deko3d::align (1048576 + (MAX_TEXTURE - 2) * 4096, DK_MEMBLOCK_ALIGNMENT)}
	                      .setFlags (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
	                      .create ();

	auto &cmdBuf = s_cmdBuf[0];

	unsigned imageIndex  = 1;
	unsigned imageOffset = 0;
	for (auto const &textureInfo : textureInfos)
	{
		struct stat st;
		if (::stat (textureInfo.path, &st) != 0)
		{
			std::fprintf (stderr, "stat(%s): %s\n", textureInfo.path, std::strerror (errno));
			std::abort ();
		}

		FILE* fp;
		fp = fopen(textureInfo.path, "r");
		if (!fp)
		{
			std::fprintf (stderr, "open(%s): %s\n", textureInfo.path, std::strerror (errno));
			std::abort ();
		}

		// read file into memory
		std::vector<char> buffer (st.st_size);
		fread(buffer.data(), buffer.size(), 1, fp);
		if (buffer.empty())
		{
			std::fprintf (stderr, "read(%s): %s\n", textureInfo.path, std::strerror (errno));
			std::abort ();
		}

		// get uncompressed size
		auto const size = ZSTD_getFrameContentSize (buffer.data (), buffer.size ());
		if (ZSTD_isError (size))
		{
			std::fprintf (stderr, "ZSTD_getFrameContentSize: %s\n", ZSTD_getErrorName (size));
			std::abort ();
		}
		//assert (size <= memBlock.getSize ());

		// wait for previous transfer to complete
		s_queue.waitIdle ();

		// decompress into transfer memblock
		auto const decoded =
		    ZSTD_decompress (memBlock.getCpuAddr (), size, buffer.data (), buffer.size ());
		if (ZSTD_isError (decoded))
		{
			std::fprintf (stderr, "ZSTD_decompress: %s\n", ZSTD_getErrorName (decoded));
			std::abort ();
		}

		// initialize texture image layout
		dk::ImageLayout layout;
		dk::ImageLayoutMaker{s_device}
		    .setFlags (0)
		    .setFormat (DkImageFormat_RGBA8_Unorm)
		    .setDimensions (textureInfo.width, textureInfo.height)
		    .initialize (layout);

		// calculate image offset
		imageOffset = imgui::deko3d::align (imageOffset, layout.getAlignment ());
		//assert (imageOffset < s_imageMemBlock.getSize ());
		//assert (s_imageMemBlock.getSize () - imageOffset >= layout.getSize ());

		// initialize image descriptor
		dk::Image image;
		image.initialize (layout, s_imageMemBlock, imageOffset);
		s_imageDescriptors[imageIndex++].initialize (image);

		// copy texture to image
		dk::ImageView imageView (image);
		cmdBuf.copyBufferToImage ({memBlock.getGpuAddr ()},
		    imageView,
		    {0, 0, 0, textureInfo.width, textureInfo.height, 1});
		s_queue.submitCommands (cmdBuf.finishList ());

		imageOffset += imgui::deko3d::align (layout.getSize (), layout.getAlignment ());
	}

	// initialize sampler descriptor
	s_samplerDescriptors[1].initialize (
	    dk::Sampler{}
	        .setFilter (DkFilter_Linear, DkFilter_Linear)
	        .setWrapMode (DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge));

	// wait for commands to complete before releasing memblocks
	s_queue.waitIdle ();
}

/// \brief Deinitialize deko3d
void deko3dExit ()
{
	// clean up all of the deko3d objects
	s_imageMemBlock      = nullptr;
	s_descriptorMemBlock = nullptr;

	for (unsigned i = 0; i < FB_NUM; ++i)
	{
		s_cmdBuf[i]      = nullptr;
		s_cmdMemBlock[i] = nullptr;
	}

	s_queue         = nullptr;
	s_swapchain     = nullptr;
	s_fbMemBlock    = nullptr;
	s_depthMemBlock = nullptr;
	s_device        = nullptr;
}

void renderer::SlowMode() {
	dkSwapchainSetSwapInterval(s_swapchain, 4);
}

void renderer::FastMode() {
	dkSwapchainSetSwapInterval(s_swapchain, 2);
}

bool renderer::init ()
{
#ifndef NDEBUG
	std::setvbuf (stderr, nullptr, _IOLBF, 0);
#endif

	if (!imgui::nx::init ())
		return false;

	deko3dInit ();
	//loadTextures ();
	imgui::deko3d::init (s_device,
	    s_queue,
	    s_cmdBuf[0],
	    s_samplerDescriptors[0],
	    s_imageDescriptors[0],
	    dkMakeTextureHandle (0, 0),
	    FB_NUM);

	return true;
}

void renderer::newFrame ()
{
	imgui::nx::newFrame ();
	ImGui::NewFrame ();
}

void renderer::render ()
{
	ImGui::Render ();

	auto &io = ImGui::GetIO ();

	if (s_width != io.DisplaySize.x || s_height != io.DisplaySize.y)
	{
		s_width  = io.DisplaySize.x;
		s_height = io.DisplaySize.y;
		rebuildSwapchain (s_width, s_height);
	}

	// get image from queue
	auto const slot = s_queue.acquireImage (s_swapchain);
	auto &cmdBuf    = s_cmdBuf[slot];

	cmdBuf.clear ();

	// bind frame/depth buffers and clear them
	dk::ImageView colorTarget{s_frameBuffers[slot]};
	dk::ImageView depthTarget{s_depthBuffer};
	cmdBuf.bindRenderTargets (&colorTarget, &depthTarget);
	cmdBuf.bindColorWriteState (dk::ColorWriteState{});
	cmdBuf.setScissors(0, DkScissor{0,0,1280,720});
	cmdBuf.clearColor (0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 0.0f);
	cmdBuf.clearDepthStencil (true, 1.0f, 0xFF, 0);
	s_queue.submitCommands (cmdBuf.finishList ());

	imgui::deko3d::render (s_device, s_queue, cmdBuf, slot);

	// wait for fragments to be completed before discarding depth/stencil buffer
	cmdBuf.barrier (DkBarrier_Fragments, 0);
	cmdBuf.discardDepthStencil ();

	// present image
	s_queue.presentImage (s_swapchain, slot);
}

void renderer::exit ()
{
	imgui::nx::exit ();

	// wait for queue to be idle
	s_queue.waitIdle ();

	imgui::deko3d::exit ();
	deko3dExit ();
}
