#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>

namespace graphics
{
	namespace programtranslator
	{
		// back-end-dependent
		utils::Ref< StringBuilder > makeshaders_vsh(
			TranslatorState* ts );
		utils::Ref< StringBuilder > makeshaders_fsh(
			TranslatorState* ts,
			utils::Ref< Value > const& value );

		void makeshaders(
			TranslatorState* ts,
			utils::Ref< StringBuilder >* pvsh,
			utils::Ref< StringBuilder >* pfsh )
		{
			utils::Ref< programtranslator::Value > value = ts->pop();
			*pvsh = makeshaders_vsh( ts );
			*pfsh = makeshaders_fsh( ts, value );
		}
	}
}

