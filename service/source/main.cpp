#include <switch.h>
#include <nxIpc/ServerHost.hpp>

#include "layoff_service.hpp"
#include "overlay_service.hpp"

extern "C" {
	extern u32 __start__;

	u32 __nx_applet_type = AppletType_None;
	u32 __nx_fs_num_sessions = 1;
	u32 __nx_fsdev_direntry_cache_size = 1;

#define INNER_HEAP_SIZE 0x20000
	size_t nx_inner_heap_size = INNER_HEAP_SIZE;
	char   nx_inner_heap[INNER_HEAP_SIZE];

	void __libnx_initheap(void) {
		void* addr = nx_inner_heap;
		size_t size = nx_inner_heap_size;

		/* Newlib */
		extern char* fake_heap_start;
		extern char* fake_heap_end;

		fake_heap_start = (char*)addr;
		fake_heap_end = (char*)addr + size;
	}
}

namespace {
	nxIpc::Server<services::OverlayService> OverlayService(1, "overlay");
	nxIpc::Server<services::LayoffService> LayoffService(15, "layoff");

	nxIpc::ServerHost Host;
}

int main(int argc, char** argv)
{
	//freopen("/file.txt", "w", stdout);

	services::OverlayService::InitializeStatics();

	Host.AddServer(&OverlayService);
	Host.AddServer(&LayoffService);
	Host.StartServer();

	services::OverlayService::FinalizeStatics();

	return 0;
}