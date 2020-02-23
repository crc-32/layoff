#include "Clients.hpp"
#include "../UI/panels/IPCControl.hpp"
#include "ErrorCodes.h"
#include <cstring>

namespace layoff::IPC {

	static std::map<LayoffIdentifier, Client> __clients;
	static Mutex clientMutex;

	void InitClients()
	{
		mutexInit(&clientMutex);
	}

	Client::Client(LayoffIdentifier id) : ID(id)
	{
		memset(&name, 0, sizeof(LayoffName));
	}

	Client::~Client()
	{

	}

	void Client::SetName(const LayoffName& name)
	{
		memcpy(&this->name, &name, sizeof(LayoffName) - 1);
	}

	ClientsLock LockClients()
	{
		return ClientsLock(__clients, clientMutex);
	}

	LayoffIdentifier CreateClient()
	{
		auto&& clients = LockClients();

		LayoffIdentifier ID = 0;
		do
			ID = 1 + rand() % INT32_MAX;
		while (!ID || clients.obj.count(ID));
		
		clients.obj.emplace(ID, Client(ID));
		return ID;
	}

	Result RemoveClient(LayoffIdentifier ID)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		clients.obj.erase(ID);
		return 0;
	}

	Result SetClientName(LayoffIdentifier ID, const LayoffName& name)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		clients.obj[ID].SetName(name);
	}

	Result GetClientUIEvent(LayoffIdentifier ID, LayoffUIEvent* evt)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		*evt = clients.obj[ID].LastUIEvent;
		clients.obj[ID].LastUIEvent = { 0 };
		return 0;
	}

	Result AddUIPanel(LayoffIdentifier id, const LayoffUIHeader& header, const u8* data, u32 len)
	{
		if (header.panelID == 0)
			return ERR_INVALID_ID;

		auto&& clients = LockClients();
		if (!clients.obj.count(id))
			return ERR_CLIENT_NOT_REGISTERED;

		Result rc = 0;
		auto& cli = clients.obj[id];

		auto&& control = UI::IPC::ParseControl(header, data, len, &rc);

		if (!control && cli.Panels.count(header.panelID))
			cli.Panels.erase(header.panelID);
		else if (control)
			cli.Panels[header.panelID] = std::move(control);

		return rc;
	}
}