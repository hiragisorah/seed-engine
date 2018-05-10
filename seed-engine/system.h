#pragma once

#include <memory>

namespace Seed
{
	class Scene;

	class System
	{
	public:
		System(std::shared_ptr<System> & self)
			: self_(self) {}

	private:
		std::weak_ptr<Scene> scene_;
		std::shared_ptr<System> & self_;

	public:
		const std::weak_ptr<Scene> & scene(void) { return this->scene_; }
		const std::shared_ptr<System> & self(void) { return this->self_; }

		void set_entity(const std::shared_ptr<Scene> & scene) { this->scene_ = scene; }

	private:
		bool Remove(void) { this->OnRemove(), this->self_.reset(); }

	public:
		virtual void OnAdd(void) {}
		virtual void OnRemove(void) {}
		virtual void Pause(void) {}
		virtual void Update(void) {}
		virtual void Always(void) {}
	};
}