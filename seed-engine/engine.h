#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "window.h"
#include "graphics.h"
#include "scene-manager.h"

namespace Seed
{
	class Engine final
	{
	private:
		std::unique_ptr<SceneManager> scene_manager_;
		std::shared_ptr<Graphics> graphics_;
		std::shared_ptr<Window> window_;

	public:
		template<class _Window, class ... Args> void set_window(const Args &... args)
		{
			this->window_ = std::make_shared<_Window>(args ...);
		}
		template<class _Graphics, class ... Args> void set_graphics(const Args &... args)
		{
			if (this->window_)
			{
				this->graphics_ = std::make_shared<_Graphics>(args ...);
				this->graphics_->set_window(this->window_);
			}
		}

	public:
		template<class _Scene, class ... Args> void Run(const Args &... args...)
		{
			if (!this->graphics_)
				std::cout << "graphicsのインスタンスが生成されていません。" << std::endl;

			this->window_->Initialize();
			this->graphics_->Initialize();
			this->scene_manager_ = std::make_unique<SceneManager>();
			this->scene_manager_->set_graphics(this->graphics_);

			this->scene_manager_->ChangeScene<_Scene>(args ...);

			while (this->window_->Run() && this->graphics_->Run() && this->scene_manager_->Run());

			this->graphics_->Finalize();
			this->window_->Finalize();
		}
	};
}