#include <iostream>
#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include "utils/number_generator.hpp"
#include "utils/math.hpp"


static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * Math::PI);
    const float b = sin(t + 0.66f * 2.0f * Math::PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

class Paddle {
public:
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::Vector2f size;
	sf::Color color;

	Paddle(sf::Vector2f newSize, sf::Vector2f initPos, sf::Vector2f initVelocity, sf::Color newCol) {
		this->pos = initPos;
		this->velocity = initVelocity;
		this->size = newSize;
		this->color = newCol;
	}

	sf::RectangleShape getShape() {
		auto shape = sf::RectangleShape();
		shape.setSize(size);
		shape.setPosition(pos);
		shape.setFillColor(color);
		return shape;
	}

	void update(float dt, float dir) {
		pos += ((velocity * dir) * dt);
	}
};

enum FruitType{
    cherry, 
    //strawberry, 
    //grape, 
    //tangerine, 
    orange, 
    apple, 
    //pear, 
    //peach, 
    //pineapple, 
    melon, 
    suika
};

int32_t main(int32_t, char*[])
{
    // Create window
    constexpr int32_t window_width  = 1000;
    constexpr int32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SUIKA GAME", sf::Style::Default, settings);
    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    Solver solver;
    Renderer renderer{window};

    // Solver configuration
    solver.setConstraint({static_cast<float>(window_width) * 0.1f, static_cast<float>(window_height) * 0.1f}, {600.0f, 600.0f});
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(frame_rate);

    // Set simulation attributes
    const float        object_spawn_delay    = 0.025f;
    const float        object_spawn_speed    = 100.0f;
    const uint32_t     max_objects_count     = 1000;
    const float        max_angle             = 1.0f;

    //FRUIT ATTRIBUTES----------
    //size
    const float        cherry_r         = 20.0f;
    const float        orange_r         = 40.0f;
    const float        apple_r          = 60.0f;
    const float        melon_r          = 80.0f;
    const float        suika_r          = 100.0f;
    //color
    const auto         cherry_c         = sf::Color::Red;
    const auto         orange_c         = sf::Color::Color(255, 165, 0); //orange
    const auto         apple_c         = sf::Color::Red;
    const auto         melon_c         = sf::Color::Green;
    const auto         suika_c         = sf::Color::Cyan;

    //Set paddle config
    auto paddleSize = sf::Vector2f(50, 20);
    auto paddleVelocity = sf::Vector2f(200, 0);
    auto paddleColor = sf::Color::Red;
	sf::Vector2f paddle1_pos = {static_cast<float>(window_width) * 0.1f, static_cast<float>(window_height) * 0.1f};

    Paddle paddle1 = Paddle(paddleSize, paddle1_pos, paddleVelocity, paddleColor);

    //Set player config
    float dropDelay = 0.6f; //amount of time between each drop
    float delayTime = dropDelay; //temp variable that iterates down
    bool isDropped = false; //did the player press the drop button or not?

    //General System config
    sf::Clock clock;
    sf::Time dt_time;
    float dt;

    // Main loop
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }

        dt_time = clock.restart();
		dt = dt_time.asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) /*&& paddle1.pos.y > 140*/) { 
			paddle1.update(dt, -1);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)/*&& paddle1.pos.y < 680*/) {
			paddle1.update(dt, 1);
		}

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){ 
            if(!isDropped){
                auto& object = solver.addObject(paddle1.pos, object_radius);
                solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{0, 1});

                isDropped = true;
            }
        }
        if(isDropped){ //the delay that prevents players from spam dropping
            if(delayTime > 0){
                delayTime -= dt;
            }
            else{
                isDropped = false;
                delayTime = dropDelay;
            }
        }

        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && solver.getObjectsCount() < max_objects_count && clock.getElapsedTime().asSeconds() >= object_spawn_delay) {
        //     clock.restart();
        //     auto&       object = solver.addObject(object_spawn_position, RNGf::getRange(object_min_radius, object_max_radius)); //spawn object
        //     const float t      = solver.getTime();
        //     const float angle  = max_angle * sin(t) + Math::PI * 0.5f; //angle based on sin wave along time t
        //     solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
        //     object.color = getRainbow(t);
        // }

        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
        window.draw(paddle1.getShape());
		window.display();
    }

    return 0;
}