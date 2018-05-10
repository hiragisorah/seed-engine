#pragma once

#include <vector>
#include <memory>

#include "entity.h"
#include "system.h"
#include "graphics.h"

namespace Seed
{
	class Scene
	{
	public:
		Scene(std::shared_ptr<Scene> & self)
			: self_(self)
		{}

	private:
		std::shared_ptr<Scene> & self_;
		std::weak_ptr<Graphics> graphics_;

	public:
		const std::shared_ptr<Scene> & self(void) { return this->self_; }
		const std::weak_ptr<Graphics> & graphics(void) { return this->graphics_; }

	public:
		void set_graphics(const std::weak_ptr<Graphics> & graphics) { this->graphics_ = graphics; }

	public:
		void Destroy(void) { this->Finalize(); this->self_.reset(); }

	public:
		virtual void Initialize(void) = 0;
		virtual void Finalize(void) {}

	private:
		std::unordered_map<std::string, std::shared_ptr<System>> systems_;

	public:
		template<class _System> const std::shared_ptr<_System> system(void)
		{
			return std::static_pointer_cast<_System>(this->systems_[typeid(_System).name()]);
		}
		template<class _System, class ... Args> const std::shared_ptr<_System> add_system(Args &&... args)
		{
			auto & target = this->systems_[typeid(_System).name()];
			target = std::make_shared<_System>(target, args ...);
			target->set_entity(this->self_);
			target->OnAdd();
			return std::static_pointer_cast<_System>(target);
		}

	private:
		std::vector<std::shared_ptr<Entity>> entities_;

	public:
		template<class _Entity, class ... Args> const std::weak_ptr<_Entity> Create(const Args &... args)
		{
			this->entities_.emplace_back(nullptr);
			auto & entity = this->entities_.back();
			entity = std::make_shared<_Entity>(entity, args ...);
			entity->set_scene(this->self_);
			entity->Initialize();
			return std::static_pointer_cast<_Entity>(entity);
		}

	public:
		void Pause(void)
		{
			for (auto && system : this->systems_)
				system.second
				? system.second->Pause()
				: this->systems_.erase(system.first);

			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Pause()
				: this->entities_.erase(this->entities_.begin() + n);
		}
		void Update(void)
		{
			for (auto && system : this->systems_)
				system.second
				? system.second->Update()
				: this->systems_.erase(system.first);

			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Update()
				: this->entities_.erase(this->entities_.begin() + n);
		}
		void Always(void)
		{
			for (auto && system : this->systems_)
				system.second
				? system.second->Always()
				: this->systems_.erase(system.first);

			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Always()
				: this->entities_.erase(this->entities_.begin() + n);
		}
	};
}