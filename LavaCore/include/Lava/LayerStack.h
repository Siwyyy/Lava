#pragma once
#include "Layer.h"

namespace Lava
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer_);
		void pushOverlay(Layer* overlay_);
		void popLayer(Layer* layer_);
		void popOverlay(Layer* overlay_);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }

	private:
		std::vector<Layer*> m_layers;
		std::vector<Layer*>::iterator m_last_layer;
	};
}
