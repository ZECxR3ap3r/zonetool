// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		SndCurve* ISoundCurve::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name.empty())
			{
				return nullptr;
			}

			auto fp = FileSystem::FileOpen("sndcurves\\" + name, "rb");
			if (fp)
			{
				ZONETOOL_INFO("Parsing sndcurve \"%s\"...", name.data());

				// parse json file
				auto size = FileSystem::FileSize(fp);
				auto bytes = FileSystem::ReadBytes(fp, size);
				nlohmann::json data = nlohmann::json::parse(bytes);

				auto asset = mem->Alloc<SndCurve>();

				auto filename = data["filename"];
				if (!filename.is_null() && !filename.empty())
				{
					asset->filename = mem->StrDup(filename.get<std::string>().data());
				}

				auto knots = data["knots"];
				if (!knots.is_null())
				{
					asset->knotCount = static_cast<unsigned short>(knots.size());

					for (std::uint16_t i = 0; i < asset->knotCount; i++)
					{
						for (std::uint8_t e = 0; e < 2; e++)
						{
							auto knot = knots[i][e];
							if (!knot.is_null())
							{
								asset->knots[i][e] = knot.get<float>();
							}
						}
					}
				}
				return asset;
			}

			return nullptr;
		}

		void ISoundCurve::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).soundcurve;
			}
		}

		void ISoundCurve::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ISoundCurve::load_depending(IZone* zone)
		{
		}

		std::string ISoundCurve::name()
		{
			return this->name_;
		}

		std::int32_t ISoundCurve::type()
		{
			return sndcurve;
		}

		void ISoundCurve::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->filename = buf->write_str(this->name());

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ISoundCurve::dump(SndCurve* asset)
		{
			auto fp = FileSystem::FileOpen("sndcurves\\"s + asset->filename, "wb");

			if (fp)
			{
				nlohmann::ordered_json j;
				j["filename"] = asset->filename;
				j["knots"] = {};
				for (std::uint16_t i = 0; i < asset->knotCount; i++)
				{
					for (std::uint8_t e = 0; e < 2; e++)
					{
						j["knots"][i][e] = asset->knots[i][e];
					}
				}

				auto json_data = j.dump(4);
				fwrite(json_data.data(), json_data.size(), 1, fp);

				FileSystem::FileClose(fp);
			}
		}
	}
}
