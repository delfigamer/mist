#include <graphics/common.hpp>
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <unordered_map>
#include <d3d9.h>

namespace graphics
{
#define ENTRY( name ) { name, #name }
	static std::unordered_map< HRESULT, char const* > const errmap = {
		ENTRY( D3D_OK ),
		ENTRY( D3DOK_NOAUTOGEN ),
		ENTRY( D3DERR_WRONGTEXTUREFORMAT ),
		ENTRY( D3DERR_UNSUPPORTEDCOLOROPERATION ),
		ENTRY( D3DERR_UNSUPPORTEDCOLORARG ),
		ENTRY( D3DERR_UNSUPPORTEDALPHAOPERATION ),
		ENTRY( D3DERR_UNSUPPORTEDALPHAARG ),
		ENTRY( D3DERR_TOOMANYOPERATIONS ),
		ENTRY( D3DERR_CONFLICTINGTEXTUREFILTER ),
		ENTRY( D3DERR_UNSUPPORTEDFACTORVALUE ),
		ENTRY( D3DERR_CONFLICTINGRENDERSTATE ),
		ENTRY( D3DERR_UNSUPPORTEDTEXTUREFILTER ),
		ENTRY( D3DERR_CONFLICTINGTEXTUREPALETTE ),
		ENTRY( D3DERR_DRIVERINTERNALERROR ),
		ENTRY( D3DERR_NOTFOUND ),
		ENTRY( D3DERR_MOREDATA ),
		ENTRY( D3DERR_DEVICELOST ),
		ENTRY( D3DERR_DEVICENOTRESET ),
		ENTRY( D3DERR_NOTAVAILABLE ),
		ENTRY( D3DERR_OUTOFVIDEOMEMORY ),
		ENTRY( D3DERR_INVALIDDEVICE ),
		ENTRY( D3DERR_INVALIDCALL ),
		ENTRY( D3DERR_DRIVERINVALIDCALL ),
		ENTRY( D3DERR_WASSTILLDRAWING ),
		ENTRY( D3DOK_NOAUTOGEN ),
		ENTRY( D3DERR_DEVICEREMOVED ),
		ENTRY( D3DERR_DEVICEHUNG ),
		ENTRY( S_NOT_RESIDENT ),
		ENTRY( S_RESIDENT_IN_SHARED_MEMORY ),
		ENTRY( S_PRESENT_MODE_CHANGED ),
		ENTRY( S_PRESENT_OCCLUDED ),
		ENTRY( D3DERR_UNSUPPORTEDOVERLAY ),
		ENTRY( D3DERR_UNSUPPORTEDOVERLAYFORMAT ),
		ENTRY( D3DERR_CANNOTPROTECTCONTENT ),
		ENTRY( D3DERR_UNSUPPORTEDCRYPTO ),
		ENTRY( D3DERR_PRESENT_STATISTICS_DISJOINT )
	};

	void checkerror_pos(
		char const* filename, char const* function, int line, HRESULT hr )
	{
		if( hr )
		{
			char buffer[ 1024 ];
			auto it = errmap.find( hr );
			if( it != errmap.end() )
			{
				snprintf(
					buffer, sizeof( buffer ),
					"Direct3D error: %s", it->second );
			}
			else
			{
				snprintf(
					buffer, sizeof( buffer ),
					"Direct3D error: %#x", uint32_t( hr ) );
			}
			throw utils::StrException(
				"[%48s:%24s@%4i]\t%s", filename, function, line, buffer );
		}
	}

	int const AttributeUsageTable[ Limits::AttributeCount ][ 2 ] = {
		{ D3DDECLUSAGE_POSITION, 0 },
		{ D3DDECLUSAGE_COLOR, 0 },
		{ D3DDECLUSAGE_TEXCOORD, 2 },
		{ D3DDECLUSAGE_TEXCOORD, 3 },
		{ D3DDECLUSAGE_TEXCOORD, 4 },
		{ D3DDECLUSAGE_TEXCOORD, 5 },
		{ D3DDECLUSAGE_TEXCOORD, 6 },
		{ D3DDECLUSAGE_TEXCOORD, 7 },
	};
}
