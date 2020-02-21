#pragma once
#include <switch.h>
#include <layoff.h>
#include "IPCLock.hpp"
#include <map>
#include "../UI/panels/IPCControl.hpp"
#include "../utils.hpp"

namespace layoff::IPC {

	void InitClients();

	struct Client
	{
		Client(Client&) = delete;
		Client& operator= (Client&) = delete;

		Client() 
		{
			PrintLn("This shouldn't get called");
		}

		Client(const LayoffIdentifier& id, const char* name);
		~Client();
		Client& operator= (Client&&) = default;
		Client(Client&&) = default;

		char name[16];
		LayoffIdentifier ID;
		
		LayoffUIEvent LastUIEvent = { 0 };

		std::map<LayoffIdentifier, UI::IPC::ControlPtr> Panels;
	};

	LayoffIdentifier CreateClient(const char name[16]);
	Result RemoveClient(const LayoffIdentifier& ID);

	Result AddUIPanel(const LayoffUIHeader& header, const u8* data, u32 len);
	Result GetClientUIEvent(const LayoffIdentifier& ID, LayoffUIEvent* evt);
	
	using ClientsLock = layoff::IPC::ObjLock<std::map<LayoffIdentifier, Client>>;
	ClientsLock LockClients();
}