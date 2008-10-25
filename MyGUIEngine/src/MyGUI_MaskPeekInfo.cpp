/*!
	@file
	@author		Albert Semenov
	@date		09/2008
	@module
*/
#include "MyGUI_MaskPeekInfo.h"
#include "MyGUI_ResourceManager.h"

#include <OgreCodec.h>
#include <OgreImageCodec.h>

#include "MyGUI_LastHeader.h"

namespace MyGUI
{

	bool MaskPeekInfo::load(const std::string& _file)
	{
		std::string ext;

		size_t pos = _file.find_last_of(".");
		if ( pos == std::string::npos ) {
			MYGUI_LOG(Error, "Unable to load image file '" << _file << "' - invalid extension.");
			return false;
		}

		while ( pos != (_file.length()-1) ) ext += _file[++pos];

		Ogre::Codec * pCodec = Ogre::Codec::getCodec(ext);
		if ( ! pCodec ) {
			MYGUI_LOG(Error, "Unable to load image file '" << _file << "' - can`t find codec for '" << ext << "' extension.");
			return false;
		}

		std::string group = ResourceManager::getInstance().getResourceGroup();
		if (!helper::isFileExist(_file, ResourceManager::getInstance().getResourceGroup())) {
			MYGUI_LOG(Error, "File '" << _file << "' not found in group'" << group << "'");
			return false;
		}

		Ogre::DataStreamPtr encoded = Ogre::ResourceGroupManager::getSingleton().openResource(_file, group);
		Ogre::Codec::DecodeResult res = pCodec->decode(encoded);
		Ogre::ImageCodec::ImageData* pData = static_cast<Ogre::ImageCodec::ImageData*>(res.second.getPointer());

		unsigned char * buffer = res.first->getPtr();
		unsigned char pixel_size = static_cast<unsigned char>(Ogre::PixelUtil::getNumElemBytes( pData->format ));

		width = (int)pData->width;
		height = (int)pData->height;
		size_t size = pData->width * pData->height;
		data.resize(size);

		pos = 0;
		for (size_t pos_pix=0; pos_pix<size; pos_pix++) {
			char is_null = 0;
			for (size_t in_pix=0; in_pix<pixel_size; in_pix++) {
				if (0xFF != buffer[pos]) is_null = 1;
				pos++;
			}
			data[pos_pix] = is_null;
		}

		encoded.setNull();
		res.first.setNull();
		res.second.setNull();
		return true;
	}

} // namespace MyGUI
