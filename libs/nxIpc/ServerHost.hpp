#pragma once
#include "Server.hpp"

#include <map>
#include <atomic>
#include <vector>
#include <numeric>
#include <cstdint>

namespace nxIpc 
{
	class ServerHost
	{
	public:
		std::atomic<bool> ShouldTerminate = false;

		void AddServer(IServer* server)
		{
			if (IsRunning)
				throw std::logic_error("Can't add a server while the host is running");

			Servers.push_back({ server, server->MaxSessions(), 0, 0 });
		}

		void StartServer() 
		{
			u32 max = std::accumulate(Servers.begin(), Servers.end(), 0, [](u32 a, const Server& b) { return a + b.MaxClients; });
			if (max >= MAX_WAIT_OBJECTS)
				throw std::logic_error("Too many clients");

			if (max <= 0)
				throw std::logic_error("No server registered");

			Handles.reserve(max);

			for (Server& s : Servers)
			{
				R_THROW(smRegisterService(&s.SHandle, s.Server->ServerName(), false, s.MaxClients));
				Handles.push_back(s.SHandle);
			}

			ShouldTerminate = false;
			IsRunning = true;

			while (true)
			{
				if (ShouldTerminate)				
					break;

				try
				{
					if (!WaitAndProcessRequest())
						break;
				}
				catch (const std::exception & ex)
				{
					LogFunction("WaitAndProcessRequest: %s\n", ex.what());
				}
			}

			TerminateServer();
		}

	protected:
		void TerminateServer()
		{
			for (Handle h : Handles)
				svcCloseHandle(h);

			Sessions.clear();
			Handles.clear();

			for (Server& s : Servers)
			{
				s.Clients = 0;
				s.SHandle = 0;
				R_THROW(smUnregisterService(s.Server->ServerName()));
			}

			ShouldTerminate = false;
			IsRunning = false;
		}

		bool WaitAndProcessRequest() 
		{
			s32 index = -1;

			Result rc = svcWaitSynchronization(&index, Handles.data(), Handles.size(), UINT64_MAX);
			if (R_FAILED(rc))
			{
				if (rc == KERNELRESULT(Cancelled))
					return false;
				else R_THROW(rc);
			}

			if (index < 0 || (size_t)index >= Handles.size())
				throw std::runtime_error("Handle index is not valid");

			Handle h = Handles[index];

			try {
				if (!Sessions.count(h))
					AcceptNewSession(h);
				else
					ProcessSession(h);
			}
			catch (const std::exception & ex)
			{
				LogFunction("%s\n", ex.what());
				if (!Sessions.count(h))
				{
					LogFunction("Accept new session failed, terminating.");
					return false;
				}
				else CloseHandle(h);
			}

			return true;
		}
	
		void AcceptNewSession(Handle handle)
		{
			Handle session = 0;
			R_THROW(svcAcceptSession(&session, handle));

			auto server = std::find_if(Servers.begin(), Servers.end(), [handle](const Server& s) {return s.SHandle == handle; });
			if (server == Servers.end())
				throw std::runtime_error("This handle is not a server");

			if (server->Clients >= server->MaxClients)
			{
				R_THROW(svcCloseHandle(session));
				LogFunction("Max number of clients reached !");
				return;
			}
			else
			{
				server->Clients++;
				Handles.push_back(session);
				Sessions[session] = { &*server, server->Server->CreateSession(session) };
			}
		}

		void ProcessSession(Handle handle)
		{
			s32 _idx;
			bool ShouldClose = false;

			R_THROW(svcReplyAndReceive(&_idx, &handle, 1, 0, UINT64_MAX));

			Request r = Request::ParseFromTLS();

			switch (r.hipc.meta.type)
			{
			case CmifCommandType_Request:
				try {
					ShouldClose = Sessions[handle].interface->ReceivedCommand(r);
				}
				catch (const std::exception & ex)
				{
					LogFunction("exception in handler: %s\n", ex.what());
					Response(R_EXCEPTION_IN_HANDLER).Finalize();
					ShouldClose = true;
				}
				break;
			case CmifCommandType_Close:
				Response().Finalize();
				ShouldClose = true;
				break;
			default:
				Response(MAKERESULT(11, 403)).Finalize();
				break;
			}

			Result rc = svcReplyAndReceive(&_idx, &handle, 0, handle, 0);

			if (rc != KERNELRESULT(TimedOut))
				R_THROW(rc);

			if (ShouldClose)
				CloseHandle(handle);
		}

		void CloseHandle(Handle handle)
		{
			Sessions[handle].server->Clients--;
			Handles.erase(std::find(Handles.begin(), Handles.end(), handle));
			R_THROW(svcCloseHandle(handle));
		}

		struct Server 
		{
			IServer* Server;
			u32 MaxClients;

			Handle SHandle;
			u32 Clients;
		};

		struct Client 
		{
			Server* server;
			std::unique_ptr<IInterface> interface;
		};

		std::vector<Server> Servers;
		std::vector<Handle> Handles;

		std::map<Handle, Client> Sessions;

		bool IsRunning = false;
	};
}