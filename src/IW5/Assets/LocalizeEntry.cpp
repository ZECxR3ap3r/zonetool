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
		LocalizeEntry* ILocalizeEntry::parse(const std::string& name, ZoneMemory* mem)
		{
			const auto path = "localizedstrings/" + name;

			if (FileSystem::FileExists(path))
			{
				ZONETOOL_INFO("Parsing localized string \"%s\"...", name.c_str());

				auto* localized = mem->Alloc<LocalizeEntry>();
				localized->name = mem->StrDup(name);

				auto* file = FileSystem::FileOpen(path, "rb"s);
				if (file)
				{
					const auto size = FileSystem::FileSize(file);
					auto data = FileSystem::ReadBytes(file, size);
					localized->localizedString = mem->StrDup(std::string(data.begin(), data.end()));

					FileSystem::FileClose(file);
				}
				return localized;
			}

			return nullptr;
		}

		void ILocalizeEntry::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).localize;
			}
		}

		void ILocalizeEntry::init(void* asset, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<LocalizeEntry*>(asset);
			this->name_ = this->asset_->name;

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).localize;
			}
		}

		void ILocalizeEntry::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ILocalizeEntry::load_depending(IZone* zone)
		{
		}

		std::string ILocalizeEntry::name()
		{
			return this->name_;
		}

		std::int32_t ILocalizeEntry::type()
		{
			return localize;
		}

		void ILocalizeEntry::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->localizedString = buf->write_str(data->localizedString);
			dest->name = buf->write_str(this->name());

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILocalizeEntry::dump(LocalizeEntry* asset)
		{
			if (asset)
			{
				auto* file = FileSystem::FileOpen("localizedstrings/"s + asset->name, "wb");
				if (file)
				{
					fwrite(asset->localizedString, strlen(asset->localizedString), 1, file);
					FileSystem::FileClose(file);
				}
			}
		}
	}
}
