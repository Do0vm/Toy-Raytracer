#include <SFML/Graphics.hpp>
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400



int main() {

	double* image = new double[WINDOW_WIDTH * WINDOW_HEIGHT *3];
	memset(image, 0.0, sizeof(double) * WINDOW_WIDTH * WINDOW_HEIGHT * 3); //render pixels

	sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Ray Tracing"); // Window to display

	sf::Image sfImage;

	sfImage.create( WINDOW_WIDTH, WINDOW_HEIGHT , sf::Color::Black);

	sf::Texture texture;
	texture.loadFromImage(sfImage);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	while (window.isOpen()) {

		texture.loadFromImage(sfImage);
		sprite.setTexture(texture);
		sf::Event sfEvent;

		while (window.pollEvent(sfEvent)) {


			if (sfEvent.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(sprite);
		window.display();
	}
}