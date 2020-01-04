#pragma once
#include <switch.h>
#include <nxExt.h>
namespace IPC {
	namespace services {

		class NotificationService {
		private:
			enum CommandId {
				NotifySimpleId = 1,
				NotifyExId = 2
			};

			static Result NotifySimple(const IpcServerRequest* r);
			static Result NotifyEx(const IpcServerRequest* r);
		public:
			static Result HandleRequest(void* arg, const IpcServerRequest* r, u8* out_data, size_t* out_dataSize);
		};

	}
}