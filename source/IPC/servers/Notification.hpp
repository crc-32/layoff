#pragma once
#include <switch.h>
#include <stratosphere.hpp>

namespace IPC {
	namespace services {

		class NotificationService final : public IServiceObject {
		private:
			enum class CommandId {
				NotifySimple = 1,
				NotifyEx = 2
			};
		private:
			Result NotifySimple(InBuffer<s8> Message);
			Result NotifyEx(InBuffer<u8> MessageData);
		public:
			DEFINE_SERVICE_DISPATCH_TABLE{
				MAKE_SERVICE_COMMAND_META(NotificationService, NotifySimple),
				MAKE_SERVICE_COMMAND_META(NotificationService, NotifyEx)
			};

			struct Notification
			{
				u16 MsgLen, TitleLen, ImgLen;
				u8 Data[];
			}
			__attribute__((packed));
		};

	}
}