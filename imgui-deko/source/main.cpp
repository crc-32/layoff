#include <switch.h>
#include <imgui.h>
#include <imgui_freetype.h>
#include "imgui_deko.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Hello" << std::endl;
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(1280, 720);
    /* ==Font init== */
	Result rc = plInitialize();
	if (R_FAILED(rc))
		fatalThrow(rc);
	
	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	if (R_FAILED(rc))
		fatalThrow(rc);
	
	//ImFontAtlasFlags_NoPowerOfTwoHeight may be needed when/if we switch to hardware rendering
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight | ImFontAtlasFlags_NoMouseCursors;
	ImFont *Font25 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f);
	ImFont *Font30 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 30.0f);
	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0u);

    ImGui_ImplDeko3D_Init();

    while(appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if (kDown & KEY_PLUS)
            break;
        
        ImGui_ImplDeko3D_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplDeko3D_RenderDrawData(ImGui::GetDrawData());
        svcSleepThread(1e+9);
    }
    ImGui_ImplDeko3D_Shutdown();
}