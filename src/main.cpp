#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <ostream>
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
class Ball {
public:
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::Vector2f size;
	sf::Color color;

	Ball() { //TEST: ball moves to the right?
		size = sf::Vector2f(20, 20);

		pos = sf::Vector2f(0, 250);
		velocity = sf::Vector2f(200, 40);

		color = sf::Color::Blue;
	}

	Ball(sf::Vector2f newSize, sf::Vector2f initPos, sf::Vector2f initVelocity, sf::Color newCol) {
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

	void update(float dt) {
		pos += velocity * dt;
        checkCollisions();
	}

    void checkCollisions(){
        //is ball touching left paddle?
        //is ball touching right paddle?
        //is ball touching up wall?
        //is ball touching down wall?
            //then figure out which side is touching, and reverse direction vector, and reflect the angle accordingly

        //is ball touching left wall?
        //is ball touching right wall?
            //then change game state
            //make ball disappear
            //flash score for 1 second
            //place ball on center screen for half second
            //then start game again 
    }

};
class Paddle {

public:
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::Vector2f size;
	sf::Color color;

	Paddle() { //TEST: ball moves to the right?
		size = sf::Vector2f(15, 120);

		pos = sf::Vector2f(300, 250);
		velocity = sf::Vector2f(0, 0);

		color = sf::Color::Yellow;
	}

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
class Wall {
public:
	sf::Vector2f pos;
	sf::Vector2f size;
	sf::Color color;

	Wall() { //TEST: ball moves to the right?
		size = sf::Vector2f(15, 120);

		pos = sf::Vector2f(300, 250);
        
		color = sf::Color::Red;
	}

	Wall(sf::Vector2f newSize, sf::Vector2f initPos, sf::Color newCol) {
		this->pos = initPos;
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
};

enum State{
    player_press_enter,
    player_num_choose,

    ball_prerelease, 
    ball_moving_regular_gameplay, 
    ball_hit_wall_so_flash,

	PAUSE,
};

enum Mode{
    OnePlayerMode, 
    TwoPlayerMode,
};

bool intersectRects(sf::FloatRect& r1, sf::FloatRect& r2){ //DOES NOT WORK, SHOULD BE AND -- DOESN'T TAKE BOTH X AND Y INTO ACCOUNT
    // return !(r1.left + r1.width <= r2.left ||
    //          r1.left >= r2.left + r2.width || 
    //          r1.top + r1.height <= r2.top ||
    //          r1.top >= r2.top + r2.height); 
    
    //std::cout << "X: " << r1.left << "Y: " << r1.top << "Width: " << r1.width << "Height" << r1.height << std::endl;
    if (r1.left + r1.width >= r2.left &&    // r1 right edge past r2 left
        r1.left <= r2.left + r2.width &&    // r1 left edge past r2 right
        r1.top + r1.height >= r2.top &&    // r1 top edge past r2 bottom
        r1.top <= r2.top + r2.height) {    // r1 bottom edge past r2 top
        return true;
  }
  return false;
}
int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "PNOG"); //set window

	sf::Clock deltaSrc;
	bool mouseDown = false;

    int p1Score = 0; 
    int p2Score = 0; 

    auto ballSize = sf::Vector2f(20, 20);
    auto ballPos = sf::Vector2f(400, 450);
	auto ballVelocity = sf::Vector2f(200, 100);
    auto ballColor = sf::Color::White;
    Ball ball = Ball(ballSize, ballPos, ballVelocity, ballColor);
    
    auto paddleSize = sf::Vector2f(5, 120);
    auto paddleVelocity = sf::Vector2f(0, 200);
    auto paddleColor = sf::Color::Red;
	auto paddle1_pos = sf::Vector2f(150, 400);
	auto paddle2_pos = sf::Vector2f(800, 400);
    Paddle paddle1 = Paddle(paddleSize, paddle1_pos, paddleVelocity, paddleColor);
    Paddle paddle2 = Paddle(paddleSize, paddle2_pos, paddleVelocity, paddleColor);
    
    Wall upWall = Wall(sf::Vector2f(1000, 40), sf::Vector2f(0,100), sf::Color::Magenta);
    Wall downWall = Wall(sf::Vector2f(1000, 40), sf::Vector2f(0,800), sf::Color::Magenta);

    Wall leftWall = Wall(sf::Vector2f(50, 800), sf::Vector2f(50, 0), sf::Color::Green); 
    Wall rightWall = Wall(sf::Vector2f(50, 800), sf::Vector2f(900, 0), sf::Color::Green); 

	Wall flashSquare = Wall(sf::Vector2f(1000, 1000), sf::Vector2f(0, 0), sf::Color::White);
	float flashTime = 1.0f;

    bool paddleTouch = false;
    bool ceilingTouch = false;
    bool leftWallTouch = false;
    bool rightWallTouch = false;

	bool paused = false;

	sf::Texture Menu1_tex;
	if(!Menu1_tex.loadFromFile("../../../start_menu1.png")){
		std::cout<<"failed to load textr1"<<std::endl;
	}
	sf::Texture Menu2_tex; 
	if(!Menu2_tex.loadFromFile("../../../start_menu2.png")){
		std::cout<<"failed to load textr2"<<std::endl;
	}

	sf::Sprite Menu1;
	Menu1.setTexture(Menu1_tex);
	sf::Sprite Menu2;
	Menu2.setTexture(Menu2_tex);

    sf::Text p1Score_Display;
	sf::Text p2Score_Display;

    sf::Font font;
    if(!font.loadFromFile("../../../ARIAL.TTF")){
        std::cout << "dang that's crazy" << std::endl;
    }
    p1Score_Display.setFont(font);
	p2Score_Display.setFont(font);

    p1Score_Display.setString("0");
	p2Score_Display.setString("0");

    p1Score_Display.setCharacterSize(36);
	p2Score_Display.setCharacterSize(36);

	p1Score_Display.setPosition(800,50); //the scores are placed on the opposite side of their respective goal 
	p2Score_Display.setPosition(150,50);

    p1Score_Display.setFillColor(sf::Color::White);
	p2Score_Display.setFillColor(sf::Color::White);

	Mode gameMode; //determines whether it's 1 player or 2 player

    //p1Score_Display.setStyle(sf::Text::Bold);

	State gameState = player_press_enter;

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

		//IF PRE-ENTER, WAIT TO PRESS ENTER
		if(gameState == player_press_enter){
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
				std::cout<<"PRESSED ENTER"<<std::endl;
				gameState = player_num_choose;
			}
		}

		//IF PRE-CHOOSE, WAIT TO CHOOSE
		if(gameState == player_num_choose){
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
				std::cout<<"PRESSED 1"<<std::endl;
				gameMode = OnePlayerMode;
				gameState = ball_prerelease;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
				std::cout<<"PRESSED 2"<<std::endl;
				gameMode = TwoPlayerMode;
				gameState = ball_prerelease;
			}
		}

		//IF PRE-RELEASE STATE, RELEASE BALL
		if(gameState == ball_prerelease){
			ball.pos = ballPos;
			//Determine Velocity X
			if(p1Score <= p2Score){ //always serve to the lowest player or P1 in case of tie
				ball.velocity.x = -200;
			}else{
				ball.velocity.x = 200;
			}
			//Determine Velocity Y
			srand(rand());
			ball.velocity.y = (rand() % 400) - 200;
			std::cout << "LAUNCH VELOCITY: " << ball.velocity.y << std::endl;

			//Reset Paddle Positions
			paddle1.pos = paddle1_pos;
			paddle2.pos = paddle2_pos;

			gameState = ball_moving_regular_gameplay;
		}

		window.clear();

		//P for pause
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)){
			if(gameState == ball_moving_regular_gameplay){
				if(!paused){
					std::cout << "P to pause" << std::endl;
					gameState = PAUSE;
				}
				paused = true;
			}
			else if (gameState == PAUSE){
				std::cout << "level 1" << std::endl; 
				if(!paused){
					std::cout << "UNPAUSE" << std::endl;
					gameState = ball_moving_regular_gameplay;
				}
				paused = true;
			}
		}else{
			paused = false;
		}

		if(gameState == ball_moving_regular_gameplay){
			//PLAYER INPUT-------------------------------------

			//Player 1 Controller (up and down) ----- NO LIMITS
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				paddle1.update(dt, -1);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				paddle1.update(dt, 1);
			}

			//Player 2 Controller (up and down) ----- NO LIMITS
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				paddle2.update(dt, -1);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				paddle2.update(dt, 1);
			}

			ball.update(dt);
			
			//PADDLE COLLISION -> REVERSE X VELOCITY
			int addVelocity = 25;
			if (ball.getShape().getGlobalBounds().intersects(paddle1.getShape().getGlobalBounds()) || 
				ball.getShape().getGlobalBounds().intersects(paddle2.getShape().getGlobalBounds())){
				if(!paddleTouch){
					std::cout<< "Hit Paddle" << std::endl;
					ball.velocity.x *= -1;
					if(ball.velocity.x > 0){
						ball.velocity.x += addVelocity;
					} else{
						ball.velocity.x -= addVelocity;
					}
				}
				paddleTouch = true; //these bools help to ensure that only 1 collision happens
			}else{
				paddleTouch = false;
			}

			//WALL COLLISION -> REVERSE Y VELOCITY
			if ((ball.getShape().getGlobalBounds().intersects(downWall.getShape().getGlobalBounds())) || 
				(ball.getShape().getGlobalBounds().intersects(upWall.getShape().getGlobalBounds()))){
				if(!ceilingTouch){
					std::cout<< "Hit Wall" << std::endl;
					ball.velocity.y *= -1;
				} 
				ceilingTouch = true;
			}else{
				ceilingTouch = false;
			}

			//GOALPOST COLLISION -> SCORE INCREASED and BALL RESET
			if (ball.getShape().getGlobalBounds().intersects(leftWall.getShape().getGlobalBounds())){ //left goal post
				if(!leftWallTouch){
					p1Score += 1;
					p1Score_Display.setString(std::to_string(p1Score));
					std::cout<< "P2 SCORE: " << p1Score << std::endl;
					//--> then pause, flash, display score, and reposition ball
					gameState = ball_hit_wall_so_flash;
				} 
				leftWallTouch = true;
			}else{
				leftWallTouch = false;
			}		
			if (ball.getShape().getGlobalBounds().intersects(rightWall.getShape().getGlobalBounds())){ //right goal post
				if(!rightWallTouch){
					p2Score += 1;
					p2Score_Display.setString(std::to_string(p2Score));
					std::cout<< "P1 SCORE: " << p2Score << std::endl;
					//--> then pause, flash, display score, and reposition ball 
					gameState = ball_hit_wall_so_flash;
				}
				rightWallTouch = true;
			}else{
				rightWallTouch = false;
			}
		}
		

		//DRAW CALLS-------------------------------------
		window.draw(p1Score_Display);
		window.draw(p2Score_Display);

		window.draw(ball.getShape());

        window.draw(paddle1.getShape());
        window.draw(paddle2.getShape());

        window.draw(upWall.getShape());
        window.draw(downWall.getShape());
        window.draw(leftWall.getShape());
        window.draw(rightWall.getShape());
        
		if(gameState == ball_hit_wall_so_flash && flashTime > 0){
			window.draw(flashSquare.getShape());
			flashTime -= dt;
		}
		else if(!(flashTime > 0)){
			flashTime = 1.0f;
			gameState = ball_prerelease;
		}

		if(gameState == player_press_enter){
			window.draw(Menu1);
		}
		if(gameState == player_num_choose){
			window.draw(Menu2);
		}

		window.display();
	}
	return 0;
}