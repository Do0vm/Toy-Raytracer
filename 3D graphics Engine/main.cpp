#include <SFML/Graphics.hpp>
#include "Library.h"
#include <vector>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400


const int numSamples = 1000;
const vec3 eye = vec3(0.f, 0.f, 5.f);
const int maxDepth = 10;
const float brightness = (2.f * 3.1415926f) * (1.f / float(numSamples));

vec3 pathTrace(Ray ray, std::vector<Object*> objects, int depth){ // Light and global illumin

	if (depth > maxDepth) {
		return vec3(0.f);



	}

	double minT = 10000000.f;
	HitInfo hi, finalHi;

	for (int i = 0; i < objects.size(); i++) {
		hi = objects[i]->hit(ray);


		if (hi.hit && hi.t < minT) {
			minT = hi.t;
			finalHi = hi;

		}
	}

	if (!finalHi.hit)
		return vec3(0.f);


	if (finalHi.emissive)
		return finalHi.colour;

	return vec3(0.f);


	}

int main() {

	std::vector<Object*> objects;
	
	Sphere sphere;
	sphere.centre = vec3(0.f, -0.7f, -0.5f);
	sphere.radius = 0.3f;
	sphere.colour = vec3(2.f);
	objects.push_back(&sphere);

	Triangle floor1 = Triangle();
	floor1.v1 = vec3(1.f, -1.f, 1.f);
	floor1.v2 = vec3(-1.f, -1.f, -1.f);
	floor1.v3 = vec3(-1.f, -1.f, 1.f);
	floor1.colour = vec3(1.f);
	objects.push_back(&floor1);

	Triangle floor2 = Triangle();
	floor2.v1 = vec3(1.f, -1.f, 1.f);
	floor2.v2 = vec3(1.f, -1.f, -1.f);
	floor2.v3 = vec3(-1.f, -1.f, -1.f);
	floor2.colour = vec3(1.f);
	objects.push_back(&floor2);

	Triangle light1 = Triangle();
	light1.v1 = vec3(0.5f, 0.99f, 0.5f);
	light1.v2 = vec3(-0.5f, 0.99f, -0.5f);
	light1.v3 = vec3(-0.5f, 0.99f, 0.5f);
	light1.colour = vec3(1.f);
	light1.emissive = true;
	
	objects.push_back(&light1);

	Triangle light2 = Triangle();
	light2.v1 = vec3(0.5f, 0.99f, 0.5f);
	light2.v2 = vec3(0.5f, 0.99f, -0.5f);
	light2.v3 = vec3(-0.5f, 0.99f, -0.5f);
	light2.colour = vec3(1.f);
	light2.emissive = true;
	objects.push_back(&light2);


	double* image = new double[WINDOW_WIDTH * WINDOW_HEIGHT *3];
	memset(image, 0.0, sizeof(double) * WINDOW_WIDTH * WINDOW_HEIGHT * 3); //render pixels
	double* p = image;
	//"I want a blank black screen that I can paint on."

	sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Ray Tracing"); // Window to display

	sf::Image sfImage;

	sfImage.create( WINDOW_WIDTH, WINDOW_HEIGHT , sf::Color::Black);
	//“Make an image the same size as my window, filled with black.”

	sf::Texture texture;
	texture.loadFromImage(sfImage);

	sf::Sprite sprite;
	sprite.setTexture(texture);
	//connecting the image to a texture (so the computer can draw it), and then putting that texture on a sprite — think of this as preparing to show your image in the window.




	//Declaring variable to prevent numerous redeclerations inside main whileloop
	//You're preparing to remember if anything was hit by ray.
	HitInfo hi, finalHi;
	Ray ray;

	float i = 0.f, j = 0.f, minT = 10000000.f, s = 0;
	int r = 0, g = 0, b = 0;
	vec3 colour = vec3(0.f);


	while (window.isOpen()) { // This runs every frame 

		if (s < numSamples) {





			p = image;






			for (int y = 0; y < WINDOW_HEIGHT; y++) { //Go through every pixel on the screen, one by one.
				for (int x = 0; x < WINDOW_WIDTH; x++) {//looping from the top-left corner to the bottom-right.

					minT = 10000000.f;//resetting things
					finalHi.hit = false;//resetting things
					hi.hit = false;//resetting things
					colour = vec3(0.f); //resetting things

					i = (2.f * float(x) / float(WINDOW_WIDTH)) - 1.f;
					j = (2.f * float(WINDOW_HEIGHT - y) / float(WINDOW_HEIGHT) - 1.f); //ScreenPositions to Normalized co ordinates

					ray.origin = vec3(i, j, 1.2f);
					ray.direction = normalize(ray.origin - eye); //position of ray shot

					for (int k = 0; k < objects.size(); k++) { //For each object in the scene, shoot a ray at it and see if it gets hit

						hi = objects[k]->hit(ray);
						if (hi.hit && hi.t < minT) {
							finalHi = hi; //If we hit something, and it’s closer than anything we hit before, remember it.
							minT = hi.t;
						}
					}

					if (finalHi.hit) {
						if (finalHi.emissive)
							colour = finalHi.colour; //If we didn’t hit anything, the color stays black


						else {
							Ray randomRay;
							randomRay.origin = finalHi.hitLocation;
							randomRay.direction = randomDirection(finalHi.normal);



							vec3 originalColour = finalHi.colour;
							vec3 newColour = pathTrace(randomRay, objects, 0);
							colour = newColour * originalColour;

							colour *= brightness;
						}
					}
					*p += colour.r;
					p++;
					*p += colour.g;
					p++;
					*p += colour.b;
					p++;
				}
			}
			p = image;

			for (int e = 0; e < WINDOW_HEIGHT; e++) {
				for (int f = 0; f < WINDOW_WIDTH; f++) {
					r = clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);
					g = clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);
					b = clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);

					sfImage.setPixel(f, e, sf::Color::Color(r, g, b));
				}
			}


			texture.loadFromImage(sfImage);
			sprite.setTexture(texture);
			s++;


		}
	
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