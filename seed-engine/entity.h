#pragma once

#include <memory>
#include <unordered_map>

#include "component.h"

namespace Seed
{
	class Scene;

	class Entity
	{
	public:
		Entity(const std::shared_ptr<Scene> & scene, const std::shared_ptr<Entity> & parent, std::shared_ptr<Entity> & self)
			: scene_(scene), parent_(parent), self_(self) {}

	private:
		const std::shared_ptr<Scene> & scene_;
		const std::shared_ptr<Entity> & parent_;
		std::shared_ptr<Entity> & self_;

	public:
		const std::shared_ptr<Scene> & scene(void) { return this->scene_; }
		const std::shared_ptr<Entity> & parent(void) { return this->parent_; }
		const std::shared_ptr<Entity> & self(void) { return this->self_; }

	public:
		void Destroy(void) { this->self_.reset(); }

	private:
		std::unordered_map<std::string, std::shared_ptr<Component>> components_;

	public:
		template<class _Component> const std::shared_ptr<_Component> component(void)
		{
			return std::static_pointer_cast<_Component>(this->components_[typeid(_Component).name()]);
		}
		template<class _Component, class ... Args> const std::shared_ptr<_Component> add_component(Args &&... args)
		{
			auto & target = this->components_[typeid(_Component).name()];
			target = std::make_shared<_Component>(this->self_, target, args ...);
			target->Initialize();
			return std::static_pointer_cast<_Component>(target);
		}

	public:
		void Pause(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Pause()
				: this->components_.erase(component.first);
		}
		void Update(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Update()
				: this->components_.erase(component.first);
		}
		void Always(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Always()
				: this->components_.erase(component.first);
		}
		void Render(const std::unique_ptr<Graphics> & graphics)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Render(graphics)
				: this->components_.erase(component.first);
		}
	};
}