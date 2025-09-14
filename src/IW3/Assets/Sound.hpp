// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		class ISound
		{
		public:
			static IW4::SoundChannel channel_to_iw4(IW3::SoundChannel snd_channel);

			static void dump(snd_alias_list_t* asset, ZoneMemory* mem);
		};
	}
}
