#pragma once
#include <nxIpc/Server.hpp>
#include <layoff.h>
#include <array>

namespace services {
	
	class LayoffService : public nxIpc::IInterface {

	private:
		using CallHandler = void (LayoffService::*)(nxIpc::Request & req);
		
		const std::array<CallHandler, 10> handlers{
			/* 0 */ nullptr,
			/* 1 */ &LayoffService::NotifySimple,
			/* 2 */ &LayoffService::NotifyEx,
			/* 3 */ &LayoffService::SetClientName,
			/* 4 */ nullptr,
			/* 5 */ &LayoffService::PushUIPanel,
			/* 6 */ nullptr,
			/* 7 */ & LayoffService::AcquireUiEvent,
			/* 8 */ & LayoffService::GetLastUiEvent,
		};

		LayoffIdentifier id;
		LayoffName name;
		Event UiEvent;
		LayoffUIEvent LastUIEvent = {};

	public:		
		void PushUIEventData(const LayoffUIEvent& evt);

		LayoffService();
		~LayoffService();

		bool ReceivedCommand(nxIpc::Request& req) override;

		void SetClientName(nxIpc::Request& req);
		void NotifySimple(nxIpc::Request& req);
		void NotifyEx(nxIpc::Request& req);
		void PushUIPanel(nxIpc::Request& req);
		void AcquireUiEvent(nxIpc::Request& req);
		void GetLastUiEvent(nxIpc::Request& req);
	};

}