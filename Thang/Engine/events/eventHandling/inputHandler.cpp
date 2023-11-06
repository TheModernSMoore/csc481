#include "inputHandler.h"

InputHandler::InputHandler();

void InputHandler::onEvent(Event *e) {
    if (e->getType() = USER_INPUT) {
        UserInput *inp = (UserInput *) e;
        // Do input handling here
        // get character, and maybe not move character
        // just update it's desired direction?
    }
}