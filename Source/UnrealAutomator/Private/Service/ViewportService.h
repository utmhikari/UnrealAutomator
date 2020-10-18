#pragma once


#include "CoreMinimal.h"

/**
 * viewport service
 */
class FViewportService
{

private:
	static bool bIsInited;
	static int32 SurfaceViewWidth;
	static int32 SurfaceViewHeight;
	static float WidthScale;
	static float HeightScale;
	static float ViewportScale;

	static bool InitDefault();
	static bool InitAndroid();
	static bool IsViewportEnabledInEngine();

public:
	FORCEINLINE static bool GetIsInited()
	{ 
		return bIsInited;
	}

	FORCEINLINE static int32 GetSurfaceViewWidth()
	{ 
		return SurfaceViewWidth;
	}

	FORCEINLINE static int32 GetSurfaceViewHeight()
	{
		return SurfaceViewHeight;
	}

	FORCEINLINE static float GetWidthScale()
	{
		return WidthScale;
	}

	FORCEINLINE static float GetHeightScale()
	{
		return HeightScale;
	}

	FORCEINLINE static float GetViewportScale()
	{
		return ViewportScale;
	}

	// init viewport data
	static bool Init();

	// force reset viewport data
	static bool Reset();
};
