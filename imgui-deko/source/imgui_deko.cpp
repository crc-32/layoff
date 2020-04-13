#include "imgui_deko.h"
#include "DekoUtils.h"

// Backend API
bool ImGui_ImplDeko3D_Init() {
    initDeko();
}


void ImGui_ImplDeko3D_Shutdown() {
    exitDeko();
}


void ImGui_ImplDeko3D_NewFrame() {
    
}


void ImGui_ImplDeko3D_RenderDrawData(ImDrawData* draw_data) {
    render();
}


/*
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyDeviceObjects();
*/