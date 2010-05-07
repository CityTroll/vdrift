#ifndef _FONT_H
#define _FONT_H

#include "texturemanager.h"
#include "optional.h"

#include <ostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>

class FONT
{
public:
	class CHARINFO
	{
		public:
			CHARINFO() : loaded(false) {}
			bool loaded;
			float x, y, width, height;
			float xoffset, yoffset, xadvance;
	};
		
private:
	TEXTUREPTR font_texture;
	
	std::vector <CHARINFO> charinfo;
	
	bool VerifyParse(const std::string & expected, const std::string & actual, const std::string & fontinfopath, std::ostream & error_output) const;
	template <typename T>
	void GetNumber(const std::string & numberstr, T & num_output) const
	{
		std::stringstream s(numberstr);
		s >> num_output;
	}
	template <typename T>
	bool Parse(const std::string & parameter, T & num_output, std::ifstream & f, const std::string & fontinfopath, std::ostream & error_output)
	{
		std::string curstr;
		f >> curstr;
		if (!VerifyParse(parameter, curstr.substr(0,parameter.size()), fontinfopath, error_output)) return false;
		GetNumber(curstr.substr(parameter.size()), num_output);
		return true;
	}
	
public:
	bool Load(
		const std::string & fontinfopath,
		const std::string & fonttexturepath,
		const std::string & texsize,
		TEXTUREMANAGER & textures,
		std::ostream & error_output,
		bool mipmap = false);

	const TEXTUREPTR GetFontTexture() const
	{
		return font_texture;
	}
	
	///returns the charinfo or nothing if the character is out of range
	optional <const CHARINFO *> GetCharInfo(unsigned int id) const
	{
		if (id < charinfo.size() && charinfo[id].loaded)
			return &charinfo[id];
		else
			return optional <const CHARINFO *> ();
	}
	
	float GetWidth(const std::string & newtext, const float newscale) const
	{
		float cursorx(0);
	
		std::vector <float> linewidth;
	
		for (unsigned int i = 0; i < newtext.size(); i++)
		{
			if (newtext[i] == '\n')
			{
				linewidth.push_back(cursorx);
				cursorx = 0;
			}
			else
			{
				optional <const FONT::CHARINFO *> cinfo = GetCharInfo(newtext[i]);
				if (cinfo)
					cursorx += (cinfo.get()->xadvance/GetFontTexture()->GetW())*newscale;
			}
		}
	
		linewidth.push_back(cursorx);
	
		float maxwidth = *std::max_element(linewidth.begin(),linewidth.end());
	
		return maxwidth;
	}
};

#endif
