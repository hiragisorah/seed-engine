#pragma once

namespace Seed
{
	class Texture
	{
	public:
		virtual unsigned int width(void) = 0;
		virtual unsigned int height(void) = 0;

	public:
		template<class _Type> void width(void) { return static_cast<_Type>(this->width()); }
		template<class _Type> void height(void) { return static_cast<_Type>(this->height()); }
	};
}