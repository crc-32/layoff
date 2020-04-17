#pragma once

#include <imgui/imgui.h>
#include "Window.hpp"
#include "../NotificationManager.hpp"

namespace layoff::UI {
	class NotificationWindow : public Window
	{
	public:
		static const u32 MaxNotifWidth = 330;
		static const u32 NotifScreenTime = 8;

		void RequestClose() override { }

		void Update() override;
		bool ShouldRender() override;

		void Update(bool IsVolumeOpened);

		~NotificationWindow() override { }
	private:
		inline void PushStyling();
		inline void PopStyiling();

		std::vector<notif::Notification> notifCache;
		inline void UpdateCache();
	};

}