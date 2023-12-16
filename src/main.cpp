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

// enum FruitType{
//     cherry, 
//     strawberry, 
//     grape, 
//     tangerine, 
//     orange, 
//     apple, 
//     //pear, 
//     //peach, 
//     //pineapple, 
//     melon, 
//     suika
// };

// //FRUIT ATTRIBUTES----------
//     //size
//     const float        cherry_r         = 10.0f;    
//     const float        strawberry_r     = 20.0f;    
//     const float        grape_r          = 25.0f;    
//     const float        tangerine_r      = 30.0f;    
//     const float        orange_r         = 40.0f;
//     const float        apple_r          = 60.0f;
//     const float        pear_r           = 65.0f;
//     const float        peach_r          = 70.0f;
//     const float        pineapple_r      = 75.0f;
//     const float        melon_r          = 80.0f;
//     const float        suika_r          = 100.0f;

//     //color
//     const sf::Color          cherry_c         = sf::Color::Color(255, 0, 0); //red
//     const sf::Color          strawberry_c         = sf::Color::Color(255, 0, 0); //red
//     const sf::Color          grape_c         = sf::Color::Color(255, 0, 0); //red
//     const sf::Color          tangerine_c         = sf::Color::Color(255, 0, 0); //red
//     const sf::Color          orange_c         = sf::Color::Color(255, 165, 0); //orange
//     const sf::Color          apple_c          = sf::Color::Red;
//     const sf::Color          pear_c          = sf::Color::Red;
//     const sf::Color          peach_c          = sf::Color::Red;
//     const sf::Color          pineapple_c          = sf::Color::Red;
//     const sf::Color          melon_c          = sf::Color::Green;
//     const sf::Color          suika_c          = sf::Color::Cyan;

// void assignFruit(FruitType type, float& radius, sf::Color& color){
//         if(type == cherry){
            
//             radius = cherry_r;
//             color = cherry_c;
//             std::cout << "color is: " << (int) cherry_c.r << " " << (int) cherry_c.g << " "<< (int) cherry_c.b << " " << (int) cherry_c.a <<std::endl;
//         }
//         else if(type == strawberry){
//             radius = strawberry_r;
//             color = strawberry_c;
//         }
//         else if(type == grape){
//             radius = grape_r;
//             color = grape_c;
//         }
//         else if(type == tangerine){
//             radius = tangerine_r;
//             color = tangerine_c;
//         }
//         else if(type == orange){
//             radius = orange_r;
//             color = orange_c;
//         }
//         else if(type == apple){
//             radius = apple_r;
//             color = apple_c;
//         }
//         else if(type == pear){
//             radius = pear_r;
//             color = pear_c;
//         }
//         else if(type == peach){
//             radius = peach_r;
//             color = peach_c;
//         }
//         else if(type == pineapple){
//             radius = pineapple_r;
//             color = pineapple_c;
//         }
//         else if(type == melon){
//             radius = melon_r;
//             color = melon_c;
//         }
//         else if(type == suika){
//             radius = suika_r;
//             color = suika_c;
//         }
//     }

enum gameState{
    Game, 
    Game_Over,
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
    solver.setConstraint({static_cast<float>(window_width) * 0.2f, static_cast<float>(window_height) * 0.2f}, {400.0f, 400.0f});
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(frame_rate);

    // Set simulation attributes
    const float        object_spawn_delay    = 0.025f;
    const float        object_spawn_speed    = 100.0f;
    const uint32_t     max_objects_count     = 1000;
    const float        max_angle             = 1.0f;

    FruitType chooseFruits[] = {cherry, strawberry, grape, tangerine, orange};

    //Set paddle config
    auto paddleSize = sf::Vector2f(50, 20);
    auto paddleVelocity = sf::Vector2f(200, 0);
    auto paddleColor = sf::Color::Red;
	sf::Vector2f paddle1_pos = {static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.2f};

    Paddle paddle1 = Paddle(paddleSize, paddle1_pos, paddleVelocity, paddleColor);

    //Set player config
    FruitType curFruit = cherry;
    float object_radius;
    sf::Color object_color;
    assignFruit(curFruit, object_radius, object_color);

    float dropDelay = 0.6f; //amount of time between each drop
    float delayTime = dropDelay; //temp variable that iterates down
    bool isDropped = false; //did the player press the drop button or not?

    //General System config
    sf::Clock clock;
    sf::Time dt_time;
    float dt;
    
    sf::Text endGameText;

    sf::Font font;
    if(!font.loadFromFile("../../../ARIAL.TTF")){
        std::cout << "dang that's crazy" << std::endl;
    }
    endGameText.setFont(font);

    endGameText.setString("GAME OVER");

    endGameText.setCharacterSize(90);

	endGameText.setPosition(static_cast<float>(window_width) * 0.23f, static_cast<float>(window_height) * 0.4f); //the scores are placed on the opposite side of their respective goal 

    endGameText.setFillColor(sf::Color::Blue);

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

        srand((unsigned) time(NULL));

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
                object.color = object_color;
                object.type = curFruit;

                //test if the delete function works
                // object.toDelete = true;
                // solver.removeObjects();
                isDropped = true;
            }
        }
        if(isDropped){ //delays player drop
            if(delayTime > 0){
                delayTime -= dt;
            }
            else{ //once the delay is over: 
                
                //curFruit increments up the ChooseFruits list until it hits the end, then loops back to the beginning
                int rand = std::rand()%sizeof(chooseFruits)/sizeof(*chooseFruits);
                //curFruit = (curFruit == sizeof(chooseFruits)/sizeof(*chooseFruits) - 1) ? chooseFruits[0] : chooseFruits[curFruit + 1];
                curFruit = (rand > sizeof(chooseFruits)/sizeof(*chooseFruits) - 1) ? chooseFruits[0] : chooseFruits[rand];
                //curFruit = cherry;

                //set the object to the fruit attributes <------ turn this into a function so we can call it on collision
                assignFruit(curFruit, object_radius, object_color); 
                
                //reset the delay
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
        if(GameOver){
            solver.removeAllObjects();
            window.draw(endGameText);
        }
		window.display();
    }

    return 0;
}