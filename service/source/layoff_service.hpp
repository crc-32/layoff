#pragma once
#include <stratosphere.hpp>
#include <layoff.h>

namespace services {

	using namespace ams;

	class LayoffService : public ams::sf::IServiceObject {

	private:
		enum class CommandId {
			SetClientName = LayoffCmdId_SetClientName,
			NotifySimple = LayoffCmdId_NotifySimple,
			NotifyEx = LayoffCmdId_NotifyEx,
			PushUIPanel = LayoffCmdId_PushUIPanel,
			AcquireUiEvent = LayoffCmdId_AcquireUiEvent,
			GetLastUiEvent = LayoffCmdId_GetLastUiEvent
		};

		LayoffIdentifier id;
	public:
		LayoffService();
		~LayoffService();

		ams::Result SetClientName(LayoffName clientName);
		ams::Result NotifySimple(SimpleNotification notification);
		ams::Result NotifyEx();
		ams::Result PushUIPanel(sf::InBuffer& buf, LayoffUIHeader header);
		ams::Result AcquireUiEvent(sf::OutCopyHandle evt);
		ams::Result GetLastUiEvent(sf::Out<LayoffUIEvent> evt);

		DEFINE_SERVICE_DISPATCH_TABLE{
			MAKE_SERVICE_COMMAND_META(NotifySimple),
			MAKE_SERVICE_COMMAND_META(NotifyEx),
			MAKE_SERVICE_COMMAND_META(SetClientName),
			MAKE_SERVICE_COMMAND_META(PushUIPanel),
			MAKE_SERVICE_COMMAND_META(AcquireUiEvent),
			MAKE_SERVICE_COMMAND_META(GetLastUiEvent),
		};
	};

}