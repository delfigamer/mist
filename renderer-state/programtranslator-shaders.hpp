#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>

namespace graphics
{
	namespace programtranslator
	{
		// back-end-dependent
		StringBuilder makeshaders_vsh(
			TranslatorState* ts );
		StringBuilder makeshaders_fsh(
			TranslatorState* ts,
			Ref< Value > const& value );

		void makeshaders(
			TranslatorState* ts,
			StringBuilder* pvsh,
			StringBuilder* pfsh )
		{
			Ref< programtranslator::Value > value = ts->pop();
			*pvsh = makeshaders_vsh( ts );
			*pfsh = makeshaders_fsh( ts, value );
		}
	}
}

