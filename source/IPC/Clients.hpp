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
		
		LayoffUIEvent LastUIEvent = { 0 };

		std::map<LayoffIdentifier, UI::IPC::ControlPtr> Panels;
	};

	LayoffIdentifier CreateClient();
	Result RemoveClient(LayoffIdentifier ID);
	Result SetClientName(LayoffIdentifier ID, const LayoffName& name);

	Result AddUIPanel(LayoffIdentifier ID, const LayoffUIHeader& header, const u8* data, u32 len);
	Result GetClientUIEvent(LayoffIdentifier ID, LayoffUIEvent* evt);
	
	using ClientsLock = layoff::IPC::ObjLock<std::map<LayoffIdentifier, Client>>;
	ClientsLock LockClients();
}