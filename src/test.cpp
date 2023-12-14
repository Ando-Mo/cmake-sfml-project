void applyConstraint()
{
    for (auto& obj : m_objects) {
       // ball to rect --------------------------------------------------
        float testX = obj.position.x;
        float testY = obj.position.y;

        // Determine which side was hit
        float xSide = (to_obj.x >= 0) ? 1.0f : -1.0f;
        float ySide = (to_obj.y >= 0) ? 1.0f : -1.0f;

        // Adjust the position based on the side hit
        obj.position = m_constraint_center - sf::Vector2f(xSide * obj.radius, ySide * obj.radius);

        // Original code to find the closest edges
        if (obj.position.x < m_constraint_center.x) {
            testX = m_constraint_center.x;
        }
        else if (obj.position.x > m_constraint_center.x + m_constraint_dimensions.x) {
            testX = m_constraint_center.x + m_constraint_dimensions.x;
        }

        if (obj.position.y < m_constraint_center.y) {
            testY = m_constraint_center.y;
        }
        else if (obj.position.y > m_constraint_center.y + m_constraint_dimensions.y) {
            testY = m_constraint_center.y + m_constraint_dimensions.y;
        }

        const sf::Vector2f to_obj_rect = { obj.position.x - testX, obj.position.y - testY };
        float distance_rect = sqrt(to_obj_rect.x * to_obj_rect.x + to_obj_rect.y * to_obj_rect.y);

        if (distance_rect <= obj.radius) {
            const sf::Vector2f n_rect = to_obj_rect / distance_rect;
            obj.position = m_constraint_center - n_rect * (obj.radius);
        }
    }
}
