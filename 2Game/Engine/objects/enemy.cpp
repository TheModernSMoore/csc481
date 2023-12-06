#include "enemy.h"

using json = nlohmann::json;

int Enemy::enemy_count = 0;

Enemy::Enemy(sf::Vector2f size)
{
    enemy_ident = enemy_count++;
    Object::setVisible();
    m_size = size;
    object_type = std::string("Enemy");
    sf::Shape::update();
}

void Enemy::setSize(sf::Vector2f size)
{
    m_size = size;
    sf::Shape::update();
}
        
const sf::Vector2f& Enemy::getSize() const
{
    return m_size;
}
        
std::size_t Enemy::getPointCount() const
{
    return 4;
}

sf::Vector2f Enemy::getPoint(std::size_t index) const
{
    switch (index) {
    case 0:
        return sf::Vector2f(0, 0);
    case 1:
        return sf::Vector2f(m_size.x, 0);
    case 2:
        return sf::Vector2f(m_size.x, m_size.y);
    case 3:
        return sf::Vector2f(0, m_size.y);
    default:
        exit(1);
    }
}

void Enemy::logic()
{
    TimeManager *timeManager = TimeManager::get();
    Timeline *localTime = timeManager->getTimelines().at(1);
    float delta_time = localTime->deltaTime();
    rel_time += delta_time;

    if (rel_time > 50) {
        rel_time = 0;

        std::srand(std::time(nullptr));
        if (std::rand() % enemy_count == enemy_ident) {
            EventManager::get()->raise(new BulletShoot(this));
        }

        if (curr_dir % 2 == 0) {
            move(0, ENEMY_SPEED * delta_time);
        } else if (curr_dir == 1) {
            move(-1 * ENEMY_SPEED * delta_time, 0);
        } else if (curr_dir == 3) {
            move(ENEMY_SPEED * delta_time, 0);
        }
        rel_pos += ENEMY_SPEED * delta_time;
        if (curr_dir % 2 == 1 && rel_pos >= 100) {
            rel_pos = 0;
            curr_dir = (curr_dir + 1) % 4;
        } else if (curr_dir % 2 == 0 && rel_pos >= 25) {
            rel_pos = 0;
            curr_dir = (curr_dir + 1) % 4;
        }
    }
}

json Enemy::toClientJSON()
{
    json output = clientJSONHelper();
    
    // Might have to get direction of velocity in this to send to client
    // server will handle direction changes, client will merely just move it in the chosen direction until told otherwise
    output["Size"] = {m_size.x, m_size.y};

    return output;
    
}