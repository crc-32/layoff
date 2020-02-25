#include <switch.h>
#include <layoff.h>
#include <queue>
#include <map>

#include "IPCLock.hpp"

namespace layoff::IPC {

	void InitClients();

	struct Client
	{
		Client(Client&) = delete;
		Client& operator= (Client&) = delete;

		Client()
		{
			//Is needed for std::map<> in GCC but should not be called here
			fatalThrow(MAKERESULT(66,66));
		}

		Client(LayoffIdentifier id);
		~Client();

		void SetName(const LayoffName& name);

		Client& operator= (Client&&) = default;
		Client(Client&&) = default;

		LayoffName name;
		LayoffIdentifier ID;

		Event UiEvent;
		LayoffUIEvent LastUIEvent = {};
	};

	LayoffIdentifier CreateClient();
	Result RemoveClient(LayoffIdentifier ID);
	Result SetClientName(LayoffIdentifier ID, const LayoffName& name);

	Result GetClientUIEventHandle(LayoffIdentifier ID, Handle* evt);
	Result GetClientUIEvent(LayoffIdentifier ID, LayoffUIEvent* evt);

	Result SignalClientUIEvent(LayoffIdentifier ID, LayoffUIEvent& evt);
	Result ClearClientUIEvent(LayoffIdentifier ID);
}