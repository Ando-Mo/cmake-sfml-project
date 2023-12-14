#pragma once
#include "solver.hpp"


class Renderer
{
public:
    explicit
    Renderer(sf::RenderTarget& target)
        : m_target{target}
    {

    }

    void render(const Solver& solver) const
    {
        // Render constraint
        const sf::Vector2f constraint_pos = solver.getConstraintPos();
        const sf::Vector2f constraint_dim = solver.getConstraintDim();

        sf::RectangleShape constraint_background{sf::Vector2f(constraint_dim.x, constraint_dim.y)};
        constraint_background.setOrigin(constraint_dim.x/2, constraint_dim.y/2);
        constraint_background.setFillColor(sf::Color::Black);
        constraint_background.setPosition(constraint_pos.x, constraint_pos.y);
        //constraint_background.setPointCount(128);
        m_target.draw(constraint_background);

        // Render objects
        sf::CircleShape circle{1.0f};
        circle.setPointCount(32);
        circle.setOrigin(1.0f, 1.0f);
        const auto& objects = solver.getObjects();
        for (const auto& obj : objects) {
            circle.setPosition(obj.position);
            circle.setScale(obj.radius, obj.radius);
            circle.setFillColor(obj.color);
            m_target.draw(circle);
        }
    }

private:
    sf::RenderTarget& m_target;
};
