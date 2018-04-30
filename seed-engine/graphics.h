#pragma once

#include <memory>

#include "window.h"

namespace Seed
{
	class Graphics
	{
	private:
		const std::unique_ptr<Window> & window_;

	public:
		Graphics(const std::unique_ptr<Window> & window) : window_(window) {}

	public:
		virtual ~Graphics(void) {}

	public:
		virtual void Initialize(void) {}
		virtual bool Run(void) { return true; }
		virtual void Finalize(void) {}
	};
}