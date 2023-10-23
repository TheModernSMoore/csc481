#pragma once

typedef struct object_body {
    bool physicsAffected;
    // void move(float offsetX, float offsetY);
} object_body;



typedef struct object_visible {
    bool in_frame;
} object_visible; // this might just have an identifier, used as boolean in draw to check if exists
// Could only send drawable objects to clients



typedef struct object_collision_area {

} object_collision_area;