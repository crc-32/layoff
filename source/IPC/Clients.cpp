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

	Client::Client(const LayoffIdentifier& id, const char* name) : ID(id)
	{
		memcpy(this->name, name, 15);
		this->name[15] = 0;
	}

	Client::~Client()
	{

	}

	ClientsLock LockClients()
	{
		return ClientsLock(__clients, clientMutex);
	}

	LayoffIdentifier CreateClient(const char name[16])
	{
		auto&& clients = LockClients();

		LayoffIdentifier ID = 0;
		do
			ID = 1 + rand() % INT32_MAX;
		while (!ID || clients.obj.count(ID));

		PrintLn("Client connected w ID " + std::to_string(ID));

		clients.obj.emplace(ID, Client(ID, name));
		return ID;
	}

	Result RemoveClient(const LayoffIdentifier& ID)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		PrintLn("Client removed ID " + std::to_string(ID));

		clients.obj.erase(ID);
		return 0;
	}

	Result GetClientUIEvent(const LayoffIdentifier& ID, LayoffUIEvent* evt)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		*evt = clients.obj[ID].LastUIEvent;
		clients.obj[ID].LastUIEvent = { 0 };
		return 0;
	}

	Result AddUIPanel(const LayoffUIHeader& header, const u8* data, u32 len)
	{
		if (header.panelID == 0)
			return ERR_INVALID_ID;

		auto&& clients = LockClients();
		if (!clients.obj.count(header.client))
			return ERR_CLIENT_NOT_REGISTERED;

		Result rc = 0;
		auto& cli = clients.obj[header.client];

		auto&& control = UI::IPC::ParseControl(header, data, len, &rc);

		if (!control && cli.Panels.count(header.panelID))
			cli.Panels.erase(header.panelID);
		else if (control)
			cli.Panels[header.panelID] = std::move(control);

		return rc;
	}
}