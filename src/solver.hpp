#pragma once
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "utils/math.hpp"

enum FruitType{
    cherry, 
    strawberry, 
    grape, 
    tangerine, 
    orange, 
    apple, 
    pear, 
    peach, 
    pineapple, 
    melon, 
    suika
};

//GLOBALS-----------------------------------------------

bool GameOver = false;

//FRUIT ATTRIBUTES----------
    //size
    const float        cherry_r         = 10.0f;    
    const float        strawberry_r     = 20.0f;    
    const float        grape_r          = 25.0f;    
    const float        tangerine_r      = 30.0f;    
    const float        orange_r         = 40.0f;
    const float        apple_r          = 60.0f;
    const float        pear_r           = 65.0f;
    const float        peach_r          = 70.0f;
    const float        pineapple_r      = 75.0f;
    const float        melon_r          = 80.0f;
    const float        suika_r          = 100.0f;

    //color
    const sf::Color          cherry_c         = sf::Color::Color(255, 0, 0); //red
    const sf::Color          strawberry_c         = sf::Color::Color(255, 0, 0); //red
    const sf::Color          grape_c         = sf::Color::Color(255, 0, 0); //red
    const sf::Color          tangerine_c         = sf::Color::Color(255, 0, 0); //red
    const sf::Color          orange_c         = sf::Color::Color(255, 165, 0); //orange
    const sf::Color          apple_c          = sf::Color::Red;
    const sf::Color          pear_c          = sf::Color::Red;
    const sf::Color          peach_c          = sf::Color::Red;
    const sf::Color          pineapple_c          = sf::Color::Red;
    const sf::Color          melon_c          = sf::Color::Green;
    const sf::Color          suika_c          = sf::Color::Cyan;

void assignFruit(FruitType type, float& radius, sf::Color& color){
        if(type == cherry){
            
            radius = cherry_r;
            color = cherry_c;
            //std::cout << "color is: " << (int) cherry_c.r << " " << (int) cherry_c.g << " "<< (int) cherry_c.b << " " << (int) cherry_c.a <<std::endl;
        }
        else if(type == strawberry){
            radius = strawberry_r;
            color = strawberry_c;
        }
        else if(type == grape){
            radius = grape_r;
            color = grape_c;
        }
        else if(type == tangerine){
            radius = tangerine_r;
            color = tangerine_c;
        }
        else if(type == orange){
            radius = orange_r;
            color = orange_c;
        }
        else if(type == apple){
            radius = apple_r;
            color = apple_c;
        }
        else if(type == pear){
            radius = pear_r;
            color = pear_c;
        }
        else if(type == peach){
            radius = peach_r;
            color = peach_c;
        }
        else if(type == pineapple){
            radius = pineapple_r;
            color = pineapple_c;
        }
        else if(type == melon){
            radius = melon_r;
            color = melon_c;
        }
        else if(type == suika){
            radius = suika_r;
            color = suika_c;
        }
    }



struct VerletObject
{
    sf::Vector2f position;
    sf::Vector2f position_last;
    sf::Vector2f acceleration;
    FruitType type;
    bool toDelete = false;
    float radius = 10.0f;
    sf::Color color = sf::Color::White;

    VerletObject() = default;
    VerletObject(sf::Vector2f position_, float radius_): 
        position{position_},
        position_last{position_},
        acceleration{0.0f, 0.0f},
        radius{radius_}{}

    bool operator==(const VerletObject& other) const{
        return(
            position == other.position &&
            position_last == other.position_last &&
            acceleration == other.acceleration &&
            type == other.type &&
            toDelete == other.toDelete &&
            radius == other.radius &&
            color == other.color
        );
    }
    void update(float dt)
    {
        // Compute how much we moved
        const sf::Vector2f velocity = position - position_last;
        // Save current position
        position_last = position;
        //Verlet integration
        position = position + velocity + acceleration * (dt * dt);
        // Reset acceleration
        acceleration  = {};
    }

    void accelerate(sf::Vector2f a)
    {
        acceleration += a;
    }

    void setVelocity(sf::Vector2f v, float dt)
    {
        position_last = position - (v * dt);
    }

    void addVelocity(sf::Vector2f v, float dt)
    {
        position_last -= v * dt;
    }

    [[nodiscard]]
    sf::Vector2f getVelocity(float dt) const
    {
        return (position - position_last) / dt;
    }
    
};


class Solver
{
public:
    Solver() = default;

    VerletObject& addObject(sf::Vector2f position, float radius)
    {
        return m_objects.emplace_back(position, radius);
    }

    void removeAllObjects(){
        m_objects.clear();
    }

    void removeObjects()
    {
        auto it = std::remove_if(m_objects.begin(), m_objects.end(), [this](VerletObject& obj) {
            return obj.toDelete;
        });
        m_objects.erase(it, m_objects.end());
    }

    void update()
    {
        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_sub_steps}; i--;) {
            applyGravity();
            checkCollisions(step_dt);
            applyConstraint();
            updateObjects(step_dt);
        }
    }

    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void setConstraint(sf::Vector2f position, sf::Vector2f dimensions)
    {
        m_constraint_center = position;
        m_constraint_dimensions = dimensions;
    }

    void setSubStepsCount(uint32_t sub_steps)
    {
        m_sub_steps = sub_steps;
    }

    void setObjectVelocity(VerletObject& object, sf::Vector2f v)
    {
        object.setVelocity(v, getStepDt());
    }

    [[nodiscard]]
    const std::vector<VerletObject>& getObjects() const
    {
        return m_objects;
    }

    [[nodiscard]]
    sf::Vector2f getConstraintPos() const
    {
        return {m_constraint_center.x, m_constraint_center.y};
    }

    [[nodiscard]]
    sf::Vector2f getConstraintDim() const
    {
        return {m_constraint_dimensions.x, m_constraint_dimensions.y};
    }

    [[nodiscard]]
    uint64_t getObjectsCount() const
    {
        return m_objects.size();
    }

    [[nodiscard]]
    float getTime() const
    {
        return m_time;
    }

    [[nodiscard]]
    float getStepDt() const
    {
        return m_frame_dt / static_cast<float>(m_sub_steps);
    }

private:
    uint32_t                  m_sub_steps          = 1;
    sf::Vector2f              m_gravity            = {0.0f, 1000.0f};
    //constraint defined
    sf::Vector2f              m_constraint_center;
    sf::Vector2f              m_constraint_dimensions;
    std::vector<VerletObject> m_objects;
    float                     m_time               = 0.0f;
    float                     m_frame_dt           = 0.0f;

    void applyGravity()
    {
        for (auto& obj : m_objects) {
            obj.accelerate(m_gravity);
        }
    }

    void checkCollisions(float dt)
    {
        const float    response_coef = 0.75f;
        const uint64_t objects_count = m_objects.size();
        // Iterate on all objects
        for (uint64_t i{0}; i < objects_count; ++i) {
            VerletObject& object_1 = m_objects[i];
            // Iterate on object involved in new collision pairs
            for (uint64_t k{i + 1}; k < objects_count; ++k) {
                VerletObject&      object_2 = m_objects[k];
                const sf::Vector2f v        = object_1.position - object_2.position;
                const float        dist2    = v.x * v.x + v.y * v.y;
                const float        min_dist = object_1.radius + object_2.radius;
                // Check overlapping
                if (dist2 < min_dist * min_dist) {
                    if(object_1.type == object_2.type && object_1.type != suika){ //if two of the same collide
                        
                        std::cout << "incoming objects are type: " << object_1.type << std::endl;
                        //collect data from collided objects
                        sf::Vector2f new_pos = (object_1.position + object_2.position)/2.f;
                        float new_rad; 
                        sf::Color new_col; 
                        FruitType new_type = FruitType(object_1.type + 1);
                        assignFruit(new_type, new_rad, new_col);

                        //Delete the two objects that caused the collision
                        //m_objects.erase(object_1);

                        object_1.toDelete = true;                        
                        object_2.toDelete = true; //the vector indexing error shows up when I try to delete the 2nd object

                        std::cout << "new type is " << new_type << std::endl;

                        //instantiate a new object at middle position
                        auto& object = addObject(new_pos, new_rad);
                        object.color = new_col;
                        object.type = new_type;

                        //setObjectVelocity(object, object_spawn_speed * sf::Vector2f{0, 1});

                                           
                    }
                    else{
                        const float        dist  = sqrt(dist2);
                        const sf::Vector2f n     = v / dist;
                        const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
                        const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
                        const float delta        = 0.5f * response_coef * (dist - min_dist);
                    

                        // Update positions
                        object_1.position -= n * (mass_ratio_2 * delta);
                        object_2.position += n * (mass_ratio_1 * delta);
                    
                    }
                }
                
            } 
        }
        removeObjects(); 
    }

    void applyConstraint()
    {
        for (auto& obj : m_objects) {//for all spawned objects
            // //ball to ball-------------------------------------------------
            // const sf::Vector2f to_obj = m_constraint_center - obj.position;//find distance by subtracting the two position vector
            // const float dist = sqrt(to_obj.x * to_obj.x + to_obj.y * to_obj.y); //and using distance formula to find the number
            // if (dist > (m_constraint_radius - obj.radius)) { //if object is inside of the circle
            //     const sf::Vector2f n = to_obj / dist;
            //     obj.position = m_constraint_center - n * (m_constraint_radius - obj.radius);//determine how much to push the object back into constraint
            // }

            //ball to rect--------------------------------------------------
            float testX = obj.position.x;
            float testY = obj.position.y;

            // which edge is closest?
            //if (cx < rx)         testX = rx;      // test left edge
            if(obj.position.x < m_constraint_center.x){ 
                testX = m_constraint_center.x;
            }
            //else if (cx > rx+rw) testX = rx+rw;   // right edge
            else if (obj.position.x > m_constraint_center.x + m_constraint_dimensions.x){
                testX = m_constraint_center.x + m_constraint_dimensions.x;
            }
            //if (cy < ry)         testY = ry;      // top edge
            if(obj.position.y < m_constraint_center.y){
                testY = m_constraint_center.y;
            }
            //else if (cy > ry+rh) testY = ry+rh;   // bottom edge
            else if (obj.position.y > m_constraint_center.y + m_constraint_dimensions.y){
                testY = m_constraint_center.y + m_constraint_dimensions.y;
            }
            // get distance from closest edges
            //float distX = cx-testX;
            //float distY = cy-testY;
            const sf::Vector2f to_obj = {obj.position.x - testX, obj.position.y - testY};
            //float distance = sqrt( (distX*distX) + (distY*distY) );
            float distance = sqrt(to_obj.x * to_obj.x + to_obj.y * to_obj.y);

            const float distLeft = fabs(to_obj.x) - (m_constraint_dimensions.x / 2.0f);
            const float distRight = (m_constraint_dimensions.x / 2.0f) - fabs(to_obj.x);
            const float distTop = fabs(to_obj.y) - (m_constraint_dimensions.y / 2.0f);
            const float distBottom = (m_constraint_dimensions.y / 2.0f) - fabs(to_obj.y);

            // const float collideSide = min(distLeft, distRight, distTop, distBottom);
            //if distance is less than the radius, you are inside the square! 
            if(distance <= obj.radius){
                //obj.color = sf::Color::Red;
            }
            else{
                
                if (testY == m_constraint_center.y){ //if you hit the top edge
                    //make end text display
                    obj.color = sf::Color::Blue;
                    GameOver = true;
                }
                sf::Vector2f n = to_obj / distance;
                //n * (whatever side you hit - radius)
                obj.position = obj.position - n * (distance - obj.radius); //* the new position is the normal vector, reversed, multiplied by the overlap of the circle onto the square
            }
        }
    }

    void updateObjects(float dt)
    {
        for (auto& obj : m_objects) {
            obj.update(dt);
        }
    }
};
