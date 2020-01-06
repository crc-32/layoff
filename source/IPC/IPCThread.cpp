#include <sstream>
#include <stratosphere.hpp>
#include "LayoffService.hpp"
#include "../utils.hpp"

Thread notif_thread;

namespace {
    constexpr ams::sm::ServiceName ServiceName = ams::sm::ServiceName::Encode("layoff");

    struct ServerOptions {
        static constexpr size_t PointerBufferSize = 0x800;
        static constexpr size_t MaxDomains = 0x40;
        static constexpr size_t MaxDomainObjects = 0x4000;
    };

    constexpr size_t MaxServers = 1;
    constexpr size_t MaxSessions = 61;

    ams::sf::hipc::ServerManager<MaxServers, ServerOptions, MaxSessions> g_server_manager;
}

static void IPCMain(void* _arg)
{
    ams::sf::hipc::ServerManager<MaxServers, ServerOptions, MaxSessions> *server_manager = (ams::sf::hipc::ServerManager<MaxServers, ServerOptions, MaxSessions>*) _arg;
    R_ASSERT(server_manager->RegisterServer<services::LayoffService>(ServiceName, MaxSessions));
    server_manager->LoopProcess();
}

namespace IPC {

    void LaunchThread() 
    {
        //From switchbrew: priority is a number 0-0x3F. Lower value means higher priority.
        //Main thread is 0x2C
        threadCreate(&notif_thread, &IPCMain, &g_server_manager, NULL, 0x4000, 0x2D, -2);
        threadStart(&notif_thread);
    }

    void RequestAndWaitExit() 
    {
        threadWaitForExit(&notif_thread);
    }
}