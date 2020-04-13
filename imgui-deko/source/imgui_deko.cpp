#include "imgui_deko.h"
#include "DekoUtils.h"

#define FBH 720
#define FBW 1280

// Backend API
bool ImGui_ImplDeko3D_Init() {
    initDeko(FBW, FBH, 1024 * sizeof(ImDrawVert), alignof(ImDrawVert), 1024 * sizeof(ImDrawIdx), alignof(ImDrawIdx));
    return true;
}


void ImGui_ImplDeko3D_Shutdown() {
    exitDeko();
}


void ImGui_ImplDeko3D_NewFrame() {
    newFrame();
}


void ImGui_ImplDeko3D_RenderDrawData(ImDrawData* draw_data) {
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        commitVb(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), alignof(ImDrawVert));
        commitIb(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), alignof(ImDrawIdx), cmd_list->IdxBuffer.Size);

        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                //if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                //    ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                //else
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
                    // Apply scissor/clipping rectangle
                    if (clip_origin_lower_left)
                        glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                    else
                        glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
#if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
                    if (g_GlVersion >= 320)
                        glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
                    else
#endif
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
                }*/
                drawElements();
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