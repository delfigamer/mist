#pragma once

#include <common/ref.hpp>
#include <common.hpp>
#include <d3d9.h>

inline ptrdiff_t addref( IUnknown* i )
{
	if( i )
	{
		return i->AddRef();
	}
	else
	{
		return -1;
	}
}

inline ptrdiff_t release( IUnknown* i )
{
	if( i )
	{
		return i->Release();
	}
	else
	{
		return -1;
	}
}
