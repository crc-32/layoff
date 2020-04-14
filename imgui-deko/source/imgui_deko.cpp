#include "imgui_deko.h"
#include "DekoUtils.h"

#define FBH 720
#define FBW 1280

// Backend API
bool ImGui_ImplDeko3D_Init() {
    initDeko(FBW, FBH, 4096 * sizeof(ImDrawVert), alignof(ImDrawVert), 4096 * sizeof(ImDrawIdx), alignof(ImDrawIdx));
    return true;
}


void ImGui_ImplDeko3D_Shutdown() {
    exitDeko();
}


void ImGui_ImplDeko3D_NewFrame() {
    newFrame();
}

void ImGui_ImplDeko3D_SetupRenderState() {
    dynBufCmds();
}


void ImGui_ImplDeko3D_RenderDrawData(ImDrawData* draw_data) {
    reAllocVb(draw_data->TotalVtxCount * sizeof(ImDrawVert), alignof(ImDrawVert));
    reAllocIb(draw_data->TotalIdxCount * sizeof(ImDrawIdx), alignof(ImDrawIdx));
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        commitVb(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), alignof(ImDrawVert));
        commitIb(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), alignof(ImDrawIdx));

        ImGui_ImplDeko3D_SetupRenderState();
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDeko3D_SetupRenderState();
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = pcmd->ClipRect.x;
                clip_rect.y = pcmd->ClipRect.y;
                clip_rect.z = pcmd->ClipRect.z;
                clip_rect.w = pcmd->ClipRect.w;

                /*if (clip_rect.x < FBW && clip_rect.y < FBH && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                }*/
                drawElements(pcmd->ElemCount, pcmd->IdxOffset, pcmd->VtxOffset);
                //svcSleepThread(1e9);
            }
        }
    }
    render();
}


/*
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplDeko3D_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplDeko3D_DestroyDeviceObjects();
*/