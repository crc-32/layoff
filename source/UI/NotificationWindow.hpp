#pragma once

#include <imgui/imgui.h>
#include "rendering/imgui_sw.hpp"
#include "Window.hpp"
#include "../NotificationManager.hpp"

namespace layoff::UI {
	class NotificationWindow : public Window
	{
	public:
		const u32 MaxNotifWidth = 330;
		const u32 NotifScreenTime = 8;

		void RequestClose() override { }

		void Update() override;
		bool ShouldRender() override;

		~NotificationWindow() override { }
	private:
		inline void PushStyling();
		inline void PopStyiling();

		std::vector<notif::Notification> notifCache;
		s64 cacheTs = 0;
		inline void UpdateCache();
	};

}