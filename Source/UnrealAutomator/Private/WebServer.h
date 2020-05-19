#pragma once

#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"

namespace UnrealAutomator
{
	class FWebServer
	{
	public:
		static void Start(uint32 Port);
		static void Stop();

	private:
		static void BindRouters(TSharedPtr<IHttpRouter> HttpRouter);
	};

}