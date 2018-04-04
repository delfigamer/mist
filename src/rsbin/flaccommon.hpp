#pragma once

#include <common.hpp>

namespace rsbin
{
	R_CLASS( name = flac )
	class Flac
	{
	public:
		R_METHOD()
		static bool isvalidformat( int bitdepth, int channels, int samplerate )
		{
			switch( bitdepth )
			{
				case 16:
				case 24:
					break;
				default:
					return false;
			}
			switch( channels )
			{
				case 1:
				case 2:
					break;
				default:
					return false;
			}
			switch( samplerate )
			{
				case 44100:
				case 48000:
					break;
				default:
					return false;
			}
			return true;
		}
	};
}
