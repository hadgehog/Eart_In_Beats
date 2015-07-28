#pragma once
#include "DeviceResources.h"
#include "..\Helpers\Thread\PPL\critical_section_guard.h"

typedef critical_section_guard<DX::DeviceResources, DX::DeviceResources::Multithread> GuardedDeviceResources;