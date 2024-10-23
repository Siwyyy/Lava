#include "Lavapch.h"
#include "LayerStack.h"

using namespace Lava;

LayerStack::LayerStack()
{
	m_last_layer = m_layers.begin();
}

LayerStack::~LayerStack()
{
	for (auto layer : m_layers)
	{
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer_)
{
	m_last_layer = m_layers.emplace(m_last_layer, layer_);
}

void LayerStack::pushOverlay(Layer* overlay_)
{
	m_layers.emplace_back(overlay_);
}

void LayerStack::popLayer(Layer* layer_)
{
	auto layer_it = std::ranges::find(m_layers, layer_);
	if (layer_it != m_layers.end())
	{
		m_layers.erase(layer_it);
		--m_last_layer;
	}
}

void LayerStack::popOverlay(Layer* overlay_) {}
