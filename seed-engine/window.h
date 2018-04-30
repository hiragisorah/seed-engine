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
		virtual bool Run(void) = 0;
		virtual void Finalize(void) = 0;

	public:
		virtual HWND hwnd(void) const = 0;
		virtual unsigned int width(void) const = 0;
		virtual unsigned int height(void) const = 0;

	public:
		template<class _Type> _Type width(void) const { return static_cast<_Type>(this->width()); }
		template<class _Type> _Type height(void) const { return static_cast<_Type>(this->height()); }
	};
}