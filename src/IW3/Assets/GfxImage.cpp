// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/GfxImage.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::GfxImage* IGfxImage::GenerateIW4Image(GfxImage* image, ZoneMemory* mem)
		{
			auto* iw4_image = mem->Alloc<IW4::GfxImage>();

			// copy image data
			iw4_image->mapType = image->mapType;
			iw4_image->semantic = image->semantic;
			iw4_image->category = image->category;
			iw4_image->dataLen1 = image->texture.loadDef->resourceSize;
			iw4_image->dataLen2 = image->texture.loadDef->resourceSize;
			iw4_image->width = image->texture.loadDef->dimensions[1]; // height actually
			iw4_image->height = image->texture.loadDef->dimensions[0]; // width actually
			iw4_image->depth = image->texture.loadDef->dimensions[2];
			iw4_image->pad = true;
			iw4_image->name = (char*)image->name;

			// alloc texture
			iw4_image->texture_2 = mem->ManualAlloc<IW4::GfxImageLoadDef_2>(16 + image->texture.loadDef->resourceSize);
			iw4_image->texture_2->levelCount = image->texture.loadDef->levelCount;
			iw4_image->texture_2->flags = image->texture.loadDef->flags;
			iw4_image->texture_2->format = image->texture.loadDef->format;
			iw4_image->texture_2->resourceSize = image->texture.loadDef->resourceSize;
			ZeroMemory(iw4_image->texture_2->pad, 3);
			std::memcpy(iw4_image->texture_2->data, image->texture.loadDef->data, image->texture.loadDef->resourceSize);

			return iw4_image;
		}
		
		void IGfxImage::dump(GfxImage* asset, ZoneMemory* mem)
		{
			if (!asset)
			{
				return;	
			}

			ZONETOOL_INFO("dumping map image %s", asset->name);
			
			// alloc IW4 image
			auto* iw4_image = IGfxImage::GenerateIW4Image(asset, mem);
			
			// dump IW4 image
			IW4::IGfxImage::dump(iw4_image);
		}
	}
}
