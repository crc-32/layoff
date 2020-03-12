#include <cstring>
#include <unordered_map>
#include "Clients.hpp"
#include <algorithm>

#include "../../source/IPC/ErrorCodes.h"

namespace layoff::IPC::clients {

	static std::unordered_map<LayoffIdentifier, services::LayoffService*> __clients;
	static Mutex clientMutex;

	using ClientsLock = layoff::IPC::ObjLock<std::unordered_map<LayoffIdentifier, services::LayoffService*>>;

	ClientsLock LockClients()
	{
		return ClientsLock(__clients, clientMutex);
	}

	void Init()
	{
		mutexInit(&clientMutex);
	}

	LayoffIdentifier CreateIdentifier(services::LayoffService* client)
	{
		auto&& clients = LockClients();
		auto& map = clients.obj;

		LayoffIdentifier ID = 0;
		do
			ID = rand() % INT32_MAX;
		while (!ID || map.count(ID));

		map[ID] = client;

		return ID;
	}

	void FreeIdentifier(LayoffIdentifier id)
	{
		LockClients().obj.erase(id);
	}

	Result PushUIEventData(LayoffIdentifier id, const LayoffUIEvent& data)
	{
		auto&& clients = LockClients();

		if (!clients.obj.count(id))
			return ERR_CLIENT_NOT_REGISTERED;

		clients.obj[id]->PushUIEventData(data);

		return 0;
	}
}