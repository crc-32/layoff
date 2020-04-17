#include <layoff.h>
#include "IPCLock.hpp"

#include "layoff_service.hpp"

namespace layoff::IPC::clients {

	void Init();

	LayoffIdentifier CreateIdentifier(services::LayoffService*);
	void FreeIdentifier(LayoffIdentifier);

	Result PushUIEventData(LayoffIdentifier, const LayoffUIEvent&);
}