// dear imgui: Renderer for deko3d

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

// Backend API
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_Init();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_RenderDrawData(ImDrawData* draw_data);

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyDeviceObjects();