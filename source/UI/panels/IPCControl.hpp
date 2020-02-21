#pragma once

#include <memory>
#include <layoff.h>

namespace layoff::UI::IPC {

	class Control
	{
	public:
		virtual void Update() = 0;
		virtual bool SignalEvent() const = 0;
		virtual ~Control() {}

		virtual LayoffUIEvent GetEvent() = 0;

		LayoffIdentifier ID;
	};

	using ControlPtr = std::unique_ptr<Control>;

	ControlPtr ParseControl(const LayoffUIHeader& header, const u8* data, const u32 len, Result* rc);
}