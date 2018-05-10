#pragma once

#include <memory>

#include "scene.h"
#include "graphics.h"

namespace Seed
{
	class SceneManager
	{
	private:
		std::weak_ptr<Graphics> graphics_;

	public:
		const std::weak_ptr<Graphics> & graphics(void) { return this->graphics_; }
		void set_graphics(const std::weak_ptr<Graphics> & graphics) { this->graphics_ = graphics; }

	private:
		std::shared_ptr<Scene> current_scene_;
		std::shared_ptr<Scene> next_scene_;

	public:
		void Pause(void) const { this->current_scene_->Pause(); }
		void Update(void) const { this->current_scene_->Update(); }
		void Always(void) const { this->current_scene_->Always(); }

	public:
		template<class _Scene, class ... Args> void ChangeScene(const Args &... args)
		{
			this->next_scene_ = std::make_shared<_Scene>(this->current_scene_, args ...);
		}

	private:
		bool pause_ = false;
		bool running_ = true;

	public:
		bool Run(void)
		{
			if (this->current_scene_)
			{
				this->pause_ ? this->Pause() : this->Update();

				this->Always();
			}
		
			if (this->next_scene_)
			{
				this->current_scene_.swap(this->next_scene_);
				this->current_scene_->set_graphics(this->graphics_);
				this->current_scene_->Initialize();
			}
			else
			{
				return (bool)this->current_scene_;
			}

			return true;
		}
	};
}