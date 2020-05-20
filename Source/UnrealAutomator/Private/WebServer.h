#pragma once

#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"

namespace UnrealAutomator
{
	class FWebServer
	{
	public:
		/**
		 * Start server
		 */
		static void Start(uint32 Port);

		/**
		 * Stop server
		 */
		static void Stop();

	private:
		/**
		 * Bind routers with handlers
		 */
		static void BindRouters(TSharedPtr<IHttpRouter> HttpRouter);
	};

}