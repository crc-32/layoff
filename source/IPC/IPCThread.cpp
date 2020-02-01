#include <sstream>
#include <stratosphere.hpp>
#include "LayoffService.hpp"
#include "../NotificationManager.hpp"
#include "../utils.hpp"

Thread notif_thread;

namespace {
    constexpr ams::sm::ServiceName ServiceName = ams::sm::ServiceName::Encode("layoff");

    struct ServerOptions {
        static constexpr size_t PointerBufferSize = 0;
        static constexpr size_t MaxDomains = 0;
        static constexpr size_t MaxDomainObjects = 0;
    };

    constexpr size_t MaxServers = 1;
    constexpr size_t MaxSessions = 61;

    ams::sf::hipc::ServerManager<MaxServers, ServerOptions, MaxSessions> g_server_manager;
}

static void IPCMain(void* _arg)
{
    R_ASSERT(g_server_manager.RegisterServer<services::LayoffService>(ServiceName, MaxSessions));
    g_server_manager.LoopProcess();
}

namespace IPC {

    void LaunchThread() 
    {
        //From switchbrew: priority is a number 0-0x3F. Lower value means higher priority.
        //Main thread is 0x2C
        threadCreate(&notif_thread, &IPCMain, NULL, NULL, 0x4000, 0x2D, -2);
        threadStart(&notif_thread);
    }

    void RequestAndWaitExit() 
    {
        threadWaitForExit(&notif_thread);
    }
}