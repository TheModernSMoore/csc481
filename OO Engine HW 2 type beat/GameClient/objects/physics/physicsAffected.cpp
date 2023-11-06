#include "physicsAffected.h"

PhysicsAffected::PhysicsAffected(float down_accel) : Object()
{
    this->down_accel = down_accel;
}

void PhysicsAffected::move(float offsetX, float offsetY)
{
    std::unique_lock<std::mutex> lock(movetex); // mutex lock to ensure that all checks for collision are still valid before movement concludes
    Shape::move(offsetX, offsetY);
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> touchers = objectManager->overlapped(this);
    if (touchers.size() > 0)
    {
        Shape::move(-offsetX, -offsetY);
        float leastx = offsetX;
        float leasty = offsetY;
        for (auto & object : touchers)
        {
            sf::Vector2f distance = this->distanceBetween(object);
            float x = distance.x;
            float y = distance.y;
            if (x != std::numeric_limits<float>::max()) {
                if (offsetX > 0) {
                    leastx = x >= 0 && x < leastx ? x : leastx;
                } else if (offsetX < 0) {
                    leastx = x <= 0 && x > leastx ? x : leastx;
                }
            }
            if (y != std::numeric_limits<float>::max()) {
                if (offsetY > 0) {
                    leasty = y >= 0 && y < leasty ? y : leasty;
                } else if (offsetY < 0) {
                    leasty = y <= 0 && y > leasty ? y : leasty;
                }
            }
        }
        Shape::move(leastx, leasty);
    }
}
