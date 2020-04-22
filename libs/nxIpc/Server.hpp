#pragma once
#include <switch.h>
#include <map>
#include <memory>

#include "Types.hpp"
#include "Exceptions.hpp"

namespace nxIpc
{
	class IInterface
	{
	public:
		virtual bool ReceivedCommand(Request& req) = 0;
		virtual ~IInterface() {}
	};

	class IServer
	{
	public:
		IServer(u32 maxSessions, const char* name)
		{
			this->maxSessions = maxSessions;
			std::strncpy(serverName.name, name, sizeof(serverName));
		}

		virtual std::unique_ptr<IInterface> CreateSession(Handle handle) = 0;
		
		const SmServiceName& ServerName() const { return serverName; }
		u32 MaxSessions() const { return maxSessions; }
	private:
		u32 maxSessions;
		SmServiceName serverName = {0};
	};

	template <typename T>
	class Server : public IServer
	{
		static_assert(std::is_base_of<IInterface, T>(), "T must implement IInterface");
	public:
		Server(u32 maxSessions, const char* ServerName) : IServer(maxSessions, ServerName)
		{
			
		}
	
		std::unique_ptr<IInterface> CreateSession(Handle handle) override
		{
			return std::make_unique<T>();
		}
	};
}