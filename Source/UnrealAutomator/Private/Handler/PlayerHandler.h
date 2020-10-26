#pragma once


#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"


class FPlayerHandler
{
public:
	/**
	 * get player inputs (actions, axis, etc)
	 */
	static TUniquePtr<FHttpServerResponse> GetPlayerInputs(const FHttpServerRequest& Request);

	/**
	 * get player info
	 */
	static TUniquePtr<FHttpServerResponse> GetPlayerInfo(const FHttpServerRequest& Request);

	/**
	 * set player location
	 */
	static TUniquePtr<FHttpServerResponse> SetPlayerLocation(const FHttpServerRequest& Request);

	/**
	 * set player rotation
	 */
	static TUniquePtr<FHttpServerResponse> SetPlayerRotation(const FHttpServerRequest& Request);

private:
	// default drop offset in set location
	static const float DEFAULT_SETLOC_DROP_OFFSET;
};
