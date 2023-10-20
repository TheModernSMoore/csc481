#pragma once

typedef struct object_body {
    bool physicsAffected;
    // void move(float offsetX, float offsetY);
} object_body;



typedef struct object_visible {
    int identifier;
    bool in_frame;
} object_visible; // this might just have an identifier, used as boolean in draw to check if exists
// Could only send drawable objects to clients



typedef struct object_collision_area {

} object_collision_area;

typedef union size {
    size_t radius;
    float size[2];
} size;

typedef struct send_struct {
    int identifier;
    bool in_frame;
    size obj_size;
    std::string object_type;
    uint8_t color[3];
    std::string img_path;
    float xpos;
    float ypos;
} send_struct;