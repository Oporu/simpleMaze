
#ifndef SIMPLEMAZE_VIGNETTE_HPP
#define SIMPLEMAZE_VIGNETTE_HPP

#include <SFML/Graphics.hpp>

class Vignette final : public sf::Drawable {
	const float radius, darkestRadius, endRadius;
	mutable sf::CircleShape shape;
public:
	Vignette(const sf::Vector2f& position, float radius, float darkestRadius, float endRadius);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};


#endif //SIMPLEMAZE_VIGNETTE_HPP
