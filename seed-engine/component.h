#pragma once

#include <memory>

namespace Seed
{
	class Entity;

	class Component
	{
	public:
		Component(std::shared_ptr<Component> & self)
			: self_(self) {}

	private:
		std::weak_ptr<Entity> entity_;
		std::shared_ptr<Component> & self_;

	public:
		const std::weak_ptr<Entity> & entity(void) { return this->entity_; }
		const std::shared_ptr<Component> & self(void) { return this->self_; }

		void set_entity(const std::shared_ptr<Entity> & entity) { this->entity_ = entity; }

	public:
		bool Remove(void) { this->OnRemove(), this->self_.reset(); }

	public:
		virtual void OnAdd(void) {}
		virtual void OnRemove(void) {}
		virtual void Pause(void) {}
		virtual void Update(void) {}
		virtual void Always(void) {}
	};
}