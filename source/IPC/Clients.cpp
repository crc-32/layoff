#include <cstring>
#include "Clients.hpp"
#include "ErrorCodes.h"
#include "../UI/panels/IPCControl.hpp"

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

	void ClientAction(const IPCClient& cli)
	{
		auto&& clients = LockClients();

		if (cli.action == OverlayAction_Connected)
		{
			if (clients.obj.count(cli.id)) //rename client
				clients.obj[cli.id].SetName(cli.name); 
			else //insert client
			{
				Client client(cli.id);
				client.SetName(cli.name);
				clients.obj.emplace(cli.id, std::move(client));
			}
		}
		else if (cli.action == OverlayAction_Disconnected)
		{
			if (clients.obj.count(cli.id))
				clients.obj.erase(cli.id);
			else
				Print("Cannot remove not registered client !");
		}
		else PrintLn("Unknown overlay action: " + std::to_string(cli.action));
	}

	void AddUIPanel(const IPCUIPush& header, const u8* data) 
	{
		if (header.header.panelID == 0)
		{
			PrintLn("Invalid UI panel id");
			return;
		}

		auto&& clients = LockClients();
		if (!clients.obj.count(header.client))
		{
			PrintLn("Invalid UI client id");
			return;
		}

		auto& cli = clients.obj[header.client];

		auto&& control = UI::IPC::ParseControl(header.header, data, header.BufferLen);

		if (!control && cli.Panels.count(header.header.panelID))
			cli.Panels.erase(header.header.panelID);
		else if (control)
			cli.Panels[header.header.panelID] = std::move(control);
	}

	void PushEvent(const Client& cli, const LayoffUIEvent& event) 
	{
		IPCUIEvent evt;
		evt.Client = cli.ID;
		evt.evt = event;
		overlayPushUiStateChange(&evt);
	}
}