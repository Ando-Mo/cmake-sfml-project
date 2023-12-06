#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
class Particle {
public:
	sf::Vector2f pixelPos;
	sf::Vector2f pixelVelocity;
	float pixelRadius;
	float timeToLive;
	sf::Color color;

	Particle() {
		pixelRadius = 32.f;
		timeToLive = 1.f;

		pixelPos = sf::Vector2f(0, 0);
		pixelVelocity = sf::Vector2f(0, 0);

		color = sf::Color::White;
	}

	Particle(float pixelRadius, sf::Vector2f initPos, sf::Vector2f initVelocity, float time, sf::Color newCol) {
		this->pixelPos = initPos;
		this->pixelVelocity = initVelocity;
		this->pixelRadius = pixelRadius;
		this->timeToLive = time;
		this->color = newCol;
	}

	sf::CircleShape getShape() {
		auto shape = sf::CircleShape();
		shape.setRadius(pixelRadius);
		shape.setPosition(pixelPos);
		shape.setFillColor(color);
		return shape;
	}

	void update(float dt) {
		pixelPos += pixelVelocity * dt;
		timeToLive -= dt;
	}

	bool isDead() {
		return timeToLive <= 0;
	}

};

int rollRandomPositiveInteger(int maxValue_E) {
	return abs(rand()) % maxValue_E;
}

float rollRandomFloat() {
	int32_t firstRes = abs(rand());
	const int32_t someDenominator = 0x7FFF;
	return float(firstRes % someDenominator) / someDenominator;
}

int main()
{

	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Budget MS Paint!"); //set window

	const int maxParticles = 64;
	const float PI = 3.141592654f;
	std::vector<std::vector<Particle>> Inputs; //create a super structure
	sf::Clock deltaSrc;
	bool mouseDown = false;

	std::vector<Particle> particles;
	particles.reserve(maxParticles);

	int currParticle;

	while (window.isOpen()) //main render loop
	{
		sf::Time dt_time = deltaSrc.restart();
		float dt = dt_time.asSeconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) //if closed event, then close window
				window.close();
		}

		currParticle = Inputs.size() - 1;

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			if (!mouseDown) {
				mouseDown = true;
				//new std::vector<Particle> list; input that intstead of overloading the poor particles list
				Inputs.push_back(particles); //instead of adding Particles, add a new list each time
				std::cout << Inputs.size() << std::endl;

				currParticle += 1;

				for (int i = 0; i < maxParticles; i++) {
					// Determine random direction
					int maxSpeed = 500;
					float randomSpeed = rollRandomPositiveInteger(maxSpeed + 1);
					float randomAngleInDegrees = rollRandomPositiveInteger(360);
					float randomAngleInRadius = randomAngleInDegrees * PI / 180.f;
					auto randomDirection = sf::Vector2f(
						cos(randomAngleInRadius),
						sin(randomAngleInRadius)
					);
					// Determine random size
					int maxRandomSize = 32;
					float randomSize = rollRandomPositiveInteger(maxRandomSize + 1);
					auto randVelocity = randomDirection * randomSpeed;
					// Roll a random lifetime
					int maxLifetime = 1;
					float randomLifetime = rollRandomFloat() * maxLifetime;
					std::cout << "length of lifetime: " << randomLifetime << std::endl;
					// Roll a random color 
					int R = rollRandomPositiveInteger(255);
					int G = rollRandomPositiveInteger(255);
					int B = rollRandomPositiveInteger(255);
					sf::Color newColor = sf::Color(R, G, B);
					// Create + add particle
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);
					auto newParticle = Particle(randomSize, mousePosF, randVelocity, randomLifetime, newColor);
					//auto newParticle = Particle(32, sf::Vector2f(500, 500), sf::Vector2f(1, 1), 5);
					Inputs[currParticle].push_back(newParticle);
				}
			}
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			mouseDown = false;
		}

		//srand(420);		

		window.clear();

		for (int i = 0; i < Inputs.size(); i++) {
			for (int j = 0; j < Inputs[i].size(); j++) {
				// Update the world state
				Inputs[i][j].update(dt);

				// Draw the darn stuff
				window.draw(Inputs[i][j].getShape());

				// delete particles that pass their lifetime	
				auto curPart = Inputs[i][j];
				if (curPart.isDead()) {
					Inputs[i].erase(Inputs[i].begin() + j);
					std::cout << "erased " << Inputs[i].size() << std::endl;
				}
			}
			/*//Update the world state
			for (int j = 0; j < Inputs[i].size(); j++) {
				Inputs[i][j].update(dt);
			}	
			// delete particles that pass their lifetime	
			for (int j = Inputs[i].size() - 1; j >= 0; j--) {
				auto curPart = Inputs[i][j];
				if (curPart.isDead()) {
					Inputs[i].erase(Inputs[i].begin() + j);
					std::cout << "erased " << Inputs[i].size() << std::endl;
				}
			}
			// Draw the darn stuff
			for (int j = 0; j < Inputs[i].size(); j++) {
				window.draw(Inputs[i][j].getShape());
			}*/
		}


		//delete any vector in Inputs that is empty
		for (auto it = Inputs.begin(); it != Inputs.end(); ) {
			if (it->empty()) {
				it = Inputs.erase(it); // Remove empty inner vector
				std::cout << "deleted" << std::endl;
			}
			else {
				++it;
			}
		}

		window.display();
	}
	return 0;
}