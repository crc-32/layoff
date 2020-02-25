#include "Clients.hpp"
#include <map>
#include <cstring>

#include "../../source/IPC/ErrorCodes.h"

namespace layoff::IPC {

	static std::map<LayoffIdentifier, Client> __clients;
	static Mutex clientMutex;

	using ClientsLock = layoff::IPC::ObjLock<std::map<LayoffIdentifier, Client>>;

	void InitClients()
	{
		mutexInit(&clientMutex);
	}

	Client::Client(LayoffIdentifier id) : ID(id)
	{
		memset(&name, 0, sizeof(LayoffName));
		Result rc = eventCreate(&UiEvent, true);
		if (R_FAILED(rc))
			fatalThrow(rc);
	}

	Client::~Client()
	{
		eventClose(&UiEvent);
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
		return 0;
	}

	Result GetClientUIEventHandle(LayoffIdentifier ID, Handle* evt)
	{
		ClientsLock&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		if (!eventActive(&clients.obj[ID].UiEvent))
			fatalThrow(MAKERESULT(66, 65));

		*evt = clients.obj[ID].UiEvent.revent;
		return 0;
	}

	Result GetClientUIEvent(LayoffIdentifier ID, LayoffUIEvent* evt)
	{
		ClientsLock&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		*evt = clients.obj[ID].LastUIEvent;
		clients.obj[ID].LastUIEvent = {};
		//eventClear(&clients.obj[ID].UiEvent);
		return 0;
	}

	Result SignalClientUIEvent(LayoffIdentifier ID, LayoffUIEvent& evt)
	{
		ClientsLock&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		clients.obj[ID].LastUIEvent = evt;
		return eventFire(&clients.obj[ID].UiEvent);
	}

	Result ClearClientUIEvent(LayoffIdentifier ID)
	{
		ClientsLock&& clients = LockClients();

		if (!clients.obj.count(ID))
			return ERR_CLIENT_NOT_REGISTERED;

		clients.obj[ID].LastUIEvent = {};
		//eventClear(&clients.obj[ID].UiEvent);
		return 0;
	}
}