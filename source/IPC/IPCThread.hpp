#pragma once
namespace ams::result
{
	bool CallFatalOnResultAssertion = true;
}

namespace layoff::IPC
{
	void LaunchThread();
	void RequestAndWaitExit();
}