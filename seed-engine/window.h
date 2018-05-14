#pragma once

#include <Windows.h>

namespace Seed
{
	class Window
	{
	public:
		Window(void) {}
		virtual ~Window(void) {}

	public:
		virtual void Initialize(void) = 0;
		virtual const bool Run(void) = 0;
		virtual void Finalize(void) = 0;

	public:
		virtual const HWND hwnd(void) const = 0;
		virtual const unsigned int width(void) const = 0;
		virtual const unsigned int height(void) const = 0;

	public:
		template<class _Type> const _Type width(void) const { return static_cast<_Type>(this->width()); }
		template<class _Type> const _Type height(void) const { return static_cast<_Type>(this->height()); }
	};
}