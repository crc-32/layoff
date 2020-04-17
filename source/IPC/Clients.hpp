#pragma once
#include <switch.h>
#include <layoff.h>
#include <map>

#include "ServiceWrappers/overlay.h"
#include "IPCLock.hpp"
#include "../utils.hpp"
#include "../UI/panels/IPCControl.hpp"

namespace layoff::IPC {

	void InitClients();

	struct Client
	{
		Client(Client&) = delete;
		Client& operator= (Client&) = delete;

		Client() 
		{
			//Is needed for std::map<> in GCC but should not be called manually
			PrintLn("Client::Client()");
		}

		Client(LayoffIdentifier id);
		~Client();

		void SetName(const LayoffName& name);

		Client& operator= (Client&&) = default;
		Client(Client&&) = default;

		LayoffName name;
		LayoffIdentifier ID;
		
		std::map<LayoffIdentifier, UI::IPC::ControlPtr> Panels;
	};

	//Adds, removes or renames clients according to the IPC request
	void ClientAction(const IPCClient& cli);

	void AddUIPanel(const IPCUIPush& header, const u8* data);
	void PushEvent(const Client& cli, const LayoffUIEvent& header);
	
	using ClientsLock = layoff::IPC::ObjLock<std::map<LayoffIdentifier, Client>>;
	ClientsLock LockClients();
}