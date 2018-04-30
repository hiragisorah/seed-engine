#pragma once

#include <vector>
#include <memory>

#include "entity.h"
#include "graphics.h"

namespace Seed
{
	class Scene
	{
	public:
		Scene(std::shared_ptr<Scene> & self) : self_(self) {}

	private:
		std::shared_ptr<Scene> & self_;

	public:
		const std::shared_ptr<Scene> & self(void) { return this->self_; }

	public:
		void Destroy(void) { this->self_.reset(); }

	public:
		virtual void Initialize(void) {}
		virtual void Finalize(void) {}

	private:
		std::vector<std::shared_ptr<Entity>> entities_;

	public:
		template<class _Entity, class ... Args> const std::shared_ptr<_Entity> Create(Args &&... args)
		{
			this->entities_.emplace_back(nullptr);
			auto & entity = this->entities_.back();
			entity = std::make_shared<_Entity>(this->self_, entity, entity, args ...);
			return std::static_pointer_cast<_Entity>(entity);
		}

	public:
		void Pause(void)
		{
			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Pause()
				: this->entities_.erase(this->entities_.begin() + n);
		}
		void Update(void)
		{
			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Update()
				: this->entities_.erase(this->entities_.begin() + n);
		}
		void Always(void)
		{
			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Always()
				: this->entities_.erase(this->entities_.begin() + n);
		}
		void Render(const std::unique_ptr<Graphics> & graphics)
		{
			for (unsigned int n = 0; n < this->entities_.size(); ++n)
				this->entities_[n]
				? this->entities_[n]->Render(graphics)
				: this->entities_.erase(this->entities_.begin() + n);
		}
	};
}