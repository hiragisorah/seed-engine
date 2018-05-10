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
		Entity(std::shared_ptr<Entity> & self)
			: self_(self) {}

	private:
		std::weak_ptr<Scene> scene_;
		std::weak_ptr<Entity> parent_;
		std::shared_ptr<Entity> & self_;

	public:
		const std::weak_ptr<Scene> & scene(void) { return this->scene_; }
		const std::weak_ptr<Entity> & parent(void) { return this->parent_; }
		const std::shared_ptr<Entity> & self(void) { return this->self_; }

		void set_scene(const std::shared_ptr<Scene> & scene) { this->scene_ = scene; }
		void set_parent(const std::shared_ptr<Entity> & parent) { this->parent_ = parent; }

	public:
		void Destroy(void) { this->Finalize(); this->self_.reset(); }

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
			target = std::make_shared<_Component>(target, args ...);
			target->set_entity(this->self_);
			target->OnAdd();
			return std::static_pointer_cast<_Component>(target);
		}

	private:
		std::vector<std::shared_ptr<Entity>> children_;

	public:
		template<class _Entity, class ... Args> const std::shared_ptr<_Entity> Create(Args &&... args)
		{
			this->children_.emplace_back(nullptr);
			auto & entity = this->children_.back();
			entity = std::make_shared<_Entity>(entity, args ...);
			entity->set_scene(this->scene_);
			entity->set_parent(this->self_);
			entity->Initialize();
			return std::static_pointer_cast<_Entity>(entity);
		}

	public:
		virtual void Initialize(void) = 0;
		virtual void Finalize(void) {}

	public:
		void Pause(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Pause()
				: this->components_.erase(component.first);

			for (unsigned int n = 0; n < this->children_.size(); ++n)
				this->children_[n]
				? this->children_[n]->Pause()
				: this->children_.erase(this->children_.begin() + n);
		}
		void Update(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Update()
				: this->components_.erase(component.first);

			for (unsigned int n = 0; n < this->children_.size(); ++n)
				this->children_[n]
				? this->children_[n]->Update()
				: this->children_.erase(this->children_.begin() + n);
		}
		void Always(void)
		{
			for (auto && component : this->components_)
				component.second
				? component.second->Always()
				: this->components_.erase(component.first);

			for (unsigned int n = 0; n < this->children_.size(); ++n)
				this->children_[n]
				? this->children_[n]->Always()
				: this->children_.erase(this->children_.begin() + n);
		}
	};
}