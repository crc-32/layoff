#include "DekoUtils.h"

#include "SampleFramework/CApplication.h"
#include "SampleFramework/CMemPool.h"
#include "SampleFramework/CShader.h"
#include "SampleFramework/CCmdMemRing.h"
#include <imgui.h>

#include <iostream>
#include <array>
#include <optional>

namespace
{

    constexpr std::array VertexAttribState =
    {
        DkVtxAttribState{ 0, 0, offsetof(ImDrawVert, pos), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(ImDrawVert, col),    DkVtxAttribSize_1x32, DkVtxAttribType_Uint, 0 },
    };

    constexpr std::array VertexBufferState =
    {
        DkVtxBufferState{ sizeof(ImDrawVert), 0 },
    };

}
static constexpr unsigned NumFramebuffers = 2;
float FramebufferWidth = 1280;
float FramebufferHeight = 720;
static constexpr unsigned StaticCmdSize = 0x10000;
static constexpr unsigned DynamicCmdSize = 0x10000;

dk::UniqueDevice device;
dk::UniqueQueue queue;

std::optional<CMemPool> pool_images;
std::optional<CMemPool> pool_code;
std::optional<CMemPool> pool_data;

dk::UniqueCmdBuf cmdbuf;
dk::UniqueCmdBuf dyncmd;
CCmdMemRing<NumFramebuffers> dynmem;

CShader vertexShader;
CShader fragmentShader;

CMemPool::Handle vertexBuffer;
CMemPool::Handle indexBuffer;

CMemPool::Handle framebuffers_mem[NumFramebuffers];
dk::Image framebuffers[NumFramebuffers];
DkCmdList framebuffer_cmdlists[NumFramebuffers];
dk::UniqueSwapchain swapchain;

DkCmdList render_cmdlist;

int indexcount;

void prepareDraw()
{
    // Initialize state structs with deko3d defaults
    dk::RasterizerState rasterizerState;
    dk::ColorState colorState;
    dk::ColorWriteState colorWriteState;

    // Configure viewport and scissor
    cmdbuf.setViewports(0, { { 0.0f, 0.0f, FramebufferWidth, FramebufferHeight, 0.0f, 1.0f } });
    cmdbuf.setScissors(0, { { 0, 0, FramebufferWidth, FramebufferHeight } });

    // Clear the color buffer
    cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 0.0f);

    // Bind state required for drawing the triangle
    cmdbuf.bindShaders(DkStageFlag_GraphicsMask, { vertexShader, fragmentShader });
    cmdbuf.bindRasterizerState(rasterizerState);
    cmdbuf.bindColorState(colorState);
    cmdbuf.bindColorWriteState(colorWriteState);
    cmdbuf.bindVtxBuffer(0, vertexBuffer.getGpuAddr(), vertexBuffer.getSize());
    cmdbuf.bindVtxAttribState(VertexAttribState);
    cmdbuf.bindVtxBufferState(VertexBufferState);
    cmdbuf.bindIdxBuffer(DkIdxFormat_Uint16, indexBuffer.getGpuAddr());
    // Finish off this command list
    render_cmdlist = cmdbuf.finishList();
}

void createFramebufferResources()
{
    // Create layout for the framebuffers
    dk::ImageLayout layout_framebuffer;
    dk::ImageLayoutMaker{device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FramebufferWidth, FramebufferHeight)
        .initialize(layout_framebuffer);

    // Create the framebuffers
    std::array<DkImage const*, NumFramebuffers> fb_array;
    uint64_t fb_size  = layout_framebuffer.getSize();
    uint32_t fb_align = layout_framebuffer.getAlignment();
    for (unsigned i = 0; i < NumFramebuffers; i ++)
    {
        // Allocate a framebuffer
        framebuffers_mem[i] = pool_images->allocate(fb_size, fb_align);
        framebuffers[i].initialize(layout_framebuffer, framebuffers_mem[i].getMemBlock(), framebuffers_mem[i].getOffset());

        // Generate a command list that binds it
        dk::ImageView colorTarget{ framebuffers[i] };
        cmdbuf.bindRenderTargets(&colorTarget);
        framebuffer_cmdlists[i] = cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        fb_array[i] = &framebuffers[i];
    }

    // Create the swapchain using the framebuffers
    swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), fb_array}.create();
}

void destroyFramebufferResources()
{
    // Return early if we have nothing to destroy
    if (!swapchain) return;

    // Make sure the queue is idle before destroying anything
    queue.waitIdle();

    // Clear the static cmdbuf, destroying the static cmdlists in the process
    cmdbuf.clear();

    // Destroy the swapchain
    swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < NumFramebuffers; i ++)
        framebuffers_mem[i].destroy();
}

void allocVb(uint32_t size, uint32_t alignment) {
    vertexBuffer = pool_data->allocate(size, alignment);
    std::cout << "Allocated VBO of size " << size << std::endl;
}

void allocIb(uint32_t size, uint32_t alignment) {
    indexBuffer = pool_data->allocate(size, alignment);
    std::cout << "Allocated IBO of size " << size << std::endl;
}

void commitVb(const void* data, uint32_t size, uint32_t alignment) {
    /*uint32_t curSize = vertexBuffer.getSize();
    if (size >= curSize) {
        std::cout << "VBO too small, increasing" << std::endl;
        vertexBuffer.destroy();
        allocVb(size * 1.5, alignment);
    }*/
    memcpy(vertexBuffer.getCpuAddr(), data, vertexBuffer.getSize());
}

void commitIb(const void* data, uint32_t size, uint32_t alignment, int count) {
    /*uint32_t curSize = indexBuffer.getSize();
    if (size >= curSize) {
        std::cout << "IBO too small, increasing" << std::endl;
        indexBuffer.destroy();
        allocIb(size * 1.5, alignment);
    }*/
    memcpy(indexBuffer.getCpuAddr(), data, indexBuffer.getSize());
    indexcount = count;
}

void initDeko(float fbH, float fbW, uint32_t initialVBOSize, uint32_t VBOAlign, uint32_t initialIBOSize, uint32_t IBOAlign)
{
    std::cout << "Deko init start" << std::endl;
    FramebufferWidth = fbW;
    FramebufferHeight = fbH;
    // Create the deko3d device
    device = dk::DeviceMaker{}.create();
    std::cout << "Created dev" << std::endl;

    // Create the main queue
    queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();
    std::cout << "Created Q" << std::endl;

    // Create the memory pools
    pool_images.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, 16*1024*1024);
    pool_code.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, 128*1024);
    pool_data.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1*1024*1024);
    std::cout << "Created pools" << std::endl;

    // Create the static command buffer and feed it freshly allocated memory
    cmdbuf = dk::CmdBufMaker{device}.create();
    CMemPool::Handle cmdmem = pool_data->allocate(StaticCmdSize);
    cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());
    std::cout << "Created static cmdbuf" << std::endl;

    // Create the dynamic command buffer and allocate memory for it
    dyncmd = dk::CmdBufMaker{device}.create();
    dynmem.allocate(*pool_data, DynamicCmdSize);
    std::cout << "Created dynamic cmdbuf" << std::endl;

    // Load the shaders
    vertexShader.load(*pool_code, "romfs:/shaders/basic_vsh.dksh");
    fragmentShader.load(*pool_code, "romfs:/shaders/color_fsh.dksh");
    std::cout << "Loaded shaders" << std::endl;

    // Load the vertex buffer
    //vertexBuffer = pool_data->allocate(sizeof(TriangleVertexData), alignof(Vertex));
    //memcpy(vertexBuffer.getCpuAddr(), TriangleVertexData.data(), vertexBuffer.getSize());
    allocVb(initialVBOSize, VBOAlign);
    allocIb(initialIBOSize, IBOAlign);

    prepareDraw();

    // Create the framebuffer resources
    createFramebufferResources();
}

void exitDeko()
{
    // Destroy the framebuffer resources
    destroyFramebufferResources();

    // Destroy the vertex buffer
    vertexBuffer.destroy();
    indexBuffer.destroy();
}

int slot;

//FIXME: hangs everything and doesn't draw
void drawElements(int count) {
    dynmem.begin(dyncmd);
    dyncmd.drawIndexed(DkPrimitive_Triangles, count, 1, 0, 0, 0);
    queue.submitCommands(dynmem.end(dyncmd));
}

void newFrame() {
    // Acquire a framebuffer from the swapchain (and wait for it to be available)
    slot = queue.acquireImage(swapchain);

    // Run the command list that attaches said framebuffer to the queue
    queue.submitCommands(framebuffer_cmdlists[slot]);
    queue.submitCommands(render_cmdlist);
}

void render()
{
    // Now that we are done rendering, present it to the screen
    queue.presentImage(swapchain, slot);
}