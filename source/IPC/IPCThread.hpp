#pragma once
namespace ams::result
{
	bool CallFatalOnResultAssertion = true;
}


namespace IPC
{
	void LaunchThread();
	void RequestAndWaitExit();
}