#pragma once

#include <memory>

namespace Seed
{
	class Entity;

	class Component
	{
	public:
		Component(const std::shared_ptr<Entity> & entity, std::shared_ptr<Component> & self)
			: entity_(entity), self_(self) {}

	private:
		const std::shared_ptr<Entity> & entity_;
		std::shared_ptr<Component> & self_;

	public:
		const std::shared_ptr<Entity> & entity(void) { return this->entity_; }
		const std::shared_ptr<Component> & self(void) { return this->self_; }

	private:
		bool Remove(void) { this->Finalize(), this->self_.reset(); }

	public:
		virtual void Initialize(void) {}
		virtual void Finalize(void) {}
		virtual void Pause(void) {}
		virtual void Update(void) {}
		virtual void Always(void) {}
		virtual void Render(const std::unique_ptr<Graphics> & graphics) {}
	};
}