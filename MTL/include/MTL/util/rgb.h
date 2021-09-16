#pragma once

#include "MTL/sdk.h"

namespace MTL {


	inline const char* rgb2hex(COLORREF col)
	{
		static char ret[12];
		char buf[12];

		wsprintfA(buf, "%06X", col);

		ret[0] = '#';
		(ret[1]) = (buf[4]);
		(ret[2]) = (buf[5]);
		(ret[3]) = (buf[2]);
		(ret[4]) = (buf[3]);
		(ret[5]) = (buf[0]);
		(ret[6]) = (buf[1]);
		ret[7] = 0;
		return ret;
	}

	inline COLORREF hex2rgb(const std::string& h)
	{
		if (h.empty())
			return 0;

		std::string in(h);

		if (in.size() < 6)
		{
			in.append(std::string(6 - in.size(), '0'));
		}

		if (in[0] != '#')
		{
			std::string tmp("#");
			tmp.append(in);
			in = tmp;
		}

		std::string hex(in);
		char buf[3];
		buf[2] = 0;

		int r, g, b;
		r = g = b = 0;

		buf[0] = hex[1];
		buf[1] = hex[2];
		sscanf_s(buf, "%x", &r);

		buf[0] = hex[3];
		buf[1] = hex[4];
		sscanf_s(buf, "%x", &g);

		buf[0] = hex[5];
		buf[1] = hex[6];
		sscanf_s(buf, "%x", &b);

		return RGB(r, g, b);
	}

}