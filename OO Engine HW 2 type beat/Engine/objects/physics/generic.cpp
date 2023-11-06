#include "generic.h"

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

Generic::Generic() : Object() {}

void Generic::move(float offsetX, float offsetY)
{
    std::unique_lock<std::mutex> lock(movetex); // mutex lock to ensure that all checks for collision are still valid before movement concludes
    Shape::move(offsetX, offsetY);
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<PhysicsAffected*> touchers = objectManager->overlappedPhysics(this);
    if (touchers.size() > 0)
    {
        Shape::move(-offsetX, -offsetY);
        for (auto & phy : touchers)
        {
            sf::Vector2f distance = this->distanceBetween(phy);
            float x = distance.x != std::numeric_limits<float>::max() ? distance.x : 0;
            float y = distance.y !=std::numeric_limits<float>::max() ? distance.y : 0;
            lock.unlock(); // this calls move on the affected object so we need to unlock the mutex.
            phy->move(offsetX - x, offsetY - y);
            lock.lock(); // lock it again
        }
        Shape::move(offsetX, offsetY);
    }
}