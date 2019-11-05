#include "PowerWindow.hpp"
using namespace layoff::UI;

//Most of this code is for test purposes from old layoff, it will likely have to be replaced
 
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

namespace PayloadReboot{
	
	#define IRAM_PAYLOAD_MAX_SIZE 0x2F000
	#define IRAM_PAYLOAD_BASE 0x40010000
	
	static u8 g_reboot_payload[IRAM_PAYLOAD_MAX_SIZE] alignas(0x1000);
	static u8 g_ff_page[0x1000]						  alignas(0x1000);
	static u8 g_work_page[0x1000]					  alignas(0x1000);
	
	void do_iram_dram_copy(void *buf, uintptr_t iram_addr, size_t size, int option) {
		memcpy(g_work_page, buf, size);
		
		SecmonArgs args = {0};
		args.X[0] = 0xF0000201;             /* smcAmsIramCopy */
		args.X[1] = (uintptr_t)g_work_page;  /* DRAM Address */
		args.X[2] = iram_addr;              /* IRAM Address */
		args.X[3] = size;                   /* Copy size */
		args.X[4] = option;                 /* 0 = Read, 1 = Write */
		svcCallSecureMonitor(&args);
		
		memcpy(buf, g_work_page, size);
	}
	
	void copy_to_iram(uintptr_t iram_addr, void *buf, size_t size) {
		do_iram_dram_copy(buf, iram_addr, size, 1);
	}
	
	void copy_from_iram(void *buf, uintptr_t iram_addr, size_t size) {
		do_iram_dram_copy(buf, iram_addr, size, 0);
	}
	
	static void clear_iram(void) {
		memset(g_ff_page, 0xFF, sizeof(g_ff_page));
		for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += sizeof(g_ff_page)) {
			copy_to_iram(IRAM_PAYLOAD_BASE + i, g_ff_page, sizeof(g_ff_page));
		}
	}
	
	void Reboot(void) {	
		splInitialize();
		clear_iram();
		
		for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += 0x1000) {
			copy_to_iram(IRAM_PAYLOAD_BASE + i, &g_reboot_payload[i], 0x1000);
		}
		
		splSetConfig((SplConfigItem)65001, 2);
		splExit();
	}
	
	bool Init()
	{
		splInitialize();
		FILE *f = fopen("sdmc:/atmosphere/reboot_payload.bin", "rb");
		if (!f)
			return false;
		fread(g_reboot_payload, 1, sizeof(g_reboot_payload), f);
		fclose(f);
		return true;
	} 
	
	// taken from https://github.com/Atmosphere-NX/libstratosphere/blob/master/include/stratosphere/utilities.hpp
	void PerformShutdownSmc() {
		SecmonArgs args = {0};
		args.X[0] = 0xC3000401; /* smcSetConfig */
		args.X[1] = 65002; /* Exosphere shutdown */
		args.X[3] = 1; /* Perform shutdown. */
		svcCallSecureMonitor(&args);
	}
}

void PowerWindow::DoUpdate()
{
	ImGui::SetCursorPosY(100);
	ImGui::SetCursorPosX((1280/2)-(500/2)); // Pos = (Screen size/2) - (Btn size/2) = Center
	
	if(ImGui::Button("Sleep Mode", ImVec2(500,78))){
		sleepConsole();
		Visible = false;
		return;
	}
	
	ImGui::NewLine();
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if (!PayloadInitFail) 
	{
		if(ImGui::Button("Reboot to payload", ImVec2(500,78)))
		{
			if (PayloadReboot::Init())
				PayloadReboot::Reboot();
			else 
				PayloadInitFail = true;
		}
	}
	else 
		ImGui::Button("Missing payload !", ImVec2(500,78));
	
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if(ImGui::Button("Power off", ImVec2(500,78)))
		PayloadReboot::PerformShutdownSmc();
	
	ImGui::NewLine();
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if(ImGui::Button("Close", ImVec2(500,78)))
		Visible = false;
}
