#pragma once
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "utils/math.hpp"

float min(float one, float two, float three, float four){
    if(one < two && one < three && one < four){
        return one;
    }
    else if(two < one && two < three && two < four){
        return two;
    }
    else if(three < one && three < two && three < four){
        return three;
    }
    else if(four < one && four < two && four < three){
        return four;
    }
}

struct VerletObject
{
    sf::Vector2f position;
    sf::Vector2f position_last;
    sf::Vector2f acceleration;

    float radius = 10.0f;
    sf::Color color = sf::Color::White;

    VerletObject() = default;
    VerletObject(sf::Vector2f position_, float radius_): 
        position{position_},
        position_last{position_},
        acceleration{0.0f, 0.0f},
        radius{radius_}{}

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
                    const float        dist  = sqrt(dist2);
                    const sf::Vector2f n     = v / dist;
                    const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
                    const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
                    const float delta        = 0.5f * response_coef * (dist - min_dist);
                    
                    //Update color <--- this actually works but is getting overridden
                    object_1.color = sf::Color::Yellow;
                    object_2.color = sf::Color::Yellow;

                    // Update positions
                    object_1.position -= n * (mass_ratio_2 * delta);
                    object_2.position += n * (mass_ratio_1 * delta);
                }
            } 
        }
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
                //obj.color = sf::Color::Blue;
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
