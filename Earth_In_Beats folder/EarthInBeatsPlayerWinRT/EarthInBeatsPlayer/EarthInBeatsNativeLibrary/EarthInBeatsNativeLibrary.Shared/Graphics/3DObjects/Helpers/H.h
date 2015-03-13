#pragma once
#include "HData.h"
#include "HSystem.h"
#include "HTime.h"
#include "HMath.h"

#include "WinRT/HDataWinRT.h"

#define WSTR1(x) L#x
#define WSTR2(x) WSTR1(x)
#define WSCONCAT(a, b) WSTR2(a) WSTR2(b)

class H{
public:
	typedef HData Data;
	typedef HSystem System;
	typedef HTime Time;
	typedef HMath Math;

	class WinRT{
	public:
		typedef HDataWinRT Data;
	};
};