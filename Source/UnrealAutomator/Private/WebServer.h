#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"

namespace UnrealAutomator
{
	class FWebServer
	{
	public:
		FWebServer();
		FWebServer(uint32 Port);
		virtual ~FWebServer();

	private:
		FHttpServerModule* HttpServerModule;
		uint32 Port;
		
		static const uint32 DEFAULT_PORT = 26016;

		void Init();
		void BindRouters();
	};
};
