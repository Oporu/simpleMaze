#include "Vignette.hpp"

void Vignette::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	float r = radius;
	while (r < endRadius) {
		r += 1.f;
		shape.setRadius(r);
		shape.setOrigin(r, r);
		const sf::Uint8 alpha = r > darkestRadius ? 255 : static_cast<sf::Uint8>(255 * r / darkestRadius);
		shape.setOutlineColor(sf::Color(0, 0, 0, alpha));
		target.draw(shape, states);
	}
}

Vignette::Vignette(const sf::Vector2f &center, const float radius, const float darkestRadius, const float endRadius)
		: radius(radius), darkestRadius(darkestRadius), endRadius(endRadius) {
	shape.setPosition(center);
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineThickness(1);
}
