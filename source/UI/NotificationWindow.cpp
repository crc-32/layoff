#define IMGUI_DEFINE_MATH_OPERATORS

#include "NotificationWindow.hpp"
#include "../utils.hpp"
#include <string>
#include "../NotificationManager.hpp"
#include <time.h>

using namespace layoff;
using namespace layoff::UI;

inline void NotificationWindow::UpdateCache()
{
	if (notif::LastNotifTs() == cacheTs) return;
	notifCache.clear();
	s64 ts = time(NULL);

	auto&& n = notif::LockNotifs();
	
	auto it = n.obj.rbegin();
	while (it != n.obj.rend() && it->ts + NotifScreenTime > ts) {
		notifCache.push_back(*it);
		it++;
	}

	//Oldest notifs end up at the end of the cache
	PrintLn("TODO: play a sound here");

	cacheTs = notif::LastNotifTs();
}

void NotificationWindow::Update()
{
	s64 ts = time(NULL);

	PushStyling();

	ImVec2 pos = {0,0};

	int RemoveIndex = -1;
	int i = 0;
	char subWinName[20] = "win";
	for (const auto& n : notifCache)
	{
		if (n.ts + NotifScreenTime < ts)
		{
			RemoveIndex = i;
			break;
		}

		itoa(i, subWinName + 3, 10);
		ImGui::Begin(subWinName, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::SetWindowPos(pos);
		if (n.HasImage())
		{
			ImGui::Text("TODO: image");
			ImGui::Text(n.message.c_str());
			ImGui::Text(n.author.c_str());
		}
		else
		{
			ImGui::Text(n.message.c_str());
			ImGui::Text(n.author.c_str());
		}
		auto sz = ImGui::GetWindowSize();
		pos = { 0, sz.y + pos.y + 1 };
		ImGui::End();
		i++;
	}

	if (RemoveIndex >= 0)
		notifCache.erase(notifCache.begin() + RemoveIndex, notifCache.end());

	PopStyiling();
}

bool NotificationWindow::ShouldRender()
{
	UpdateCache();
	return notifCache.size() != 0;
}

inline void NotificationWindow::PushStyling()
{
	ImGui::Begin("Notif", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
	ImGui::SetWindowPos({ 0, 0 });
	ImGui::SetWindowSize({ MaxNotifWidth, 720 });
}

inline void NotificationWindow::PopStyiling()
{
	ImGui::End();
}