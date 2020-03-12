#include <switch.h>
#include <stratosphere.hpp>
#include <vapours.hpp>

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

	void __libnx_initheap(void);
	void __appInit(void);
	void __appExit(void);
}

namespace ams {

	ncm::ProgramId CurrentProgramId = { 0x01006C61796F6666ul }; //01 00 layoff

	namespace result {
		bool CallFatalOnResultAssertion = true;
	}
}

using namespace ams;

void __libnx_initheap(void) {
	void* addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end = (char*)addr + size;
}

void __appInit(void) {
	hos::SetVersionForLibnx();
}

void __appExit(void) {

}

namespace {

	using ServerOptions = sf::hipc::DefaultServerManagerOptions;

	constexpr sm::ServiceName LayoffServiceName = sm::ServiceName::Encode("layoff");
	constexpr size_t          LayoffMaxSessions = 15;

	constexpr sm::ServiceName OverlayServiceName = sm::ServiceName::Encode("overlay");
	constexpr size_t          OverlayMaxSessions = 1;

	constexpr size_t NumServers = 2;
	constexpr size_t NumSessions = LayoffMaxSessions + OverlayMaxSessions;

	sf::hipc::ServerManager<NumServers, ServerOptions, NumSessions> g_server_manager;
}

int main(int argc, char** argv)
{
	ams::Result rc = g_server_manager.RegisterServer<services::OverlayService>(OverlayServiceName, OverlayMaxSessions);
	if (rc.IsFailure())
		fatalThrow(rc.GetValue());
	
	rc = g_server_manager.RegisterServer<services::LayoffService>(LayoffServiceName, LayoffMaxSessions);
	if (rc.IsFailure())
		fatalThrow(rc.GetValue());

	services::OverlayService::InitializeStatics();

	g_server_manager.LoopProcess();

	services::OverlayService::FinalizeStatics();

	return 0;
}