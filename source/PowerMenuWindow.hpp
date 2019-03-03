#pragma once
#include "UI/UI.hpp"
#include "UI/imgui_sdl.h"
#include <string>

Result sleepConsole() {
    Service Srv;
    Result rc;

    rc = smGetService(&Srv, "spsm");
    if (R_FAILED(rc))
        return rc;

    IpcCommand c;
    ipcInitialize(&c);

    struct CmdStruct{
        u64 magic;
        u64 cmdid;
    } *cmdStruct;

    cmdStruct = (CmdStruct*) ipcPrepareHeader(&c, sizeof(*cmdStruct));

    cmdStruct->magic = SFCI_MAGIC;
    cmdStruct->cmdid = 1;

    rc = serviceIpcDispatch(&Srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct Resp{
            u64 magic;
            u64 result;
        } *resp = (Resp*) r.Raw;

        rc = resp->result;
    }

    return rc;
}

Result powerShutdown(bool reboot) {
	Service Srv;
	Result rc;
	rc = smGetService(&Srv, "appletOE");
	
    if (R_FAILED(rc))
        return rc;

    IpcCommand c;
    ipcInitialize(&c);

    struct CmdStruct{
        u64 magic;
        u64 cmdid;
    } *cmdStruct;

    cmdStruct = (CmdStruct*) ipcPrepareHeader(&c, sizeof(*cmdStruct));

    cmdStruct->magic = SFCI_MAGIC;
    cmdStruct->cmdid = reboot ? 71 : 70;

    rc = serviceIpcDispatch(&Srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct Resp{
            u64 magic;
            u64 result;
        } *resp = (Resp*) r.Raw;

        rc = resp->result;
    }

    return rc;
}

class PowerMenuWindow
{
public:
	bool Draw()
	{
		if (!ImGui::Begin("POWER menu", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration))
		{
			ImGui::End();
			return false;
		}
        ImGui::SetWindowFocus("POWER menu");
        ImGui::SetWindowPos(ImVec2(0,0));
        ImGui::SetWindowSize(ImVec2(1280,720));
		ImGui::SetCursorPosY(100);
		ImGui::SetCursorPosX((1280/2)-(500/2)); // Pos = (Screen size/2) - (Btn size/2) = Center
		if(ImGui::Button("Sleep Mode", ImVec2(500,78))){
			sleepConsole();
		}
		ImGui::NewLine();
		ImGui::SetCursorPosX((1280/2)-(500/2));

		if(ImGui::Button("Reboot", ImVec2(500,78))){
			powerShutdown(true);
		}
		ImGui::SetCursorPosX((1280/2)-(500/2));
		if(ImGui::Button("Power off", ImVec2(500,78))){
            powerShutdown(false);
        }
		ImGui::NewLine();
		ImGui::SetCursorPosX((1280/2)-(500/2));
		if(ImGui::Button("Close", ImVec2(500,78))){
			ImGui::End();
			return false;
		}
		ImGui::End();
		return true;
	}
};