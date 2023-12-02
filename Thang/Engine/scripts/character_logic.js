var DOWN_ACCEL = 0.3
var TERM_VEL = -6


// Uses Object0 as hopefully there will always be an Object0
// to find the Object guid that is listed in the static curr_guid of Object
var character = gethandle(Object0.curr_guid) 

//  This is the logic for non-input based movement


if (objectsAreAbove(character)) {
    character.velY = -DOWN_ACCEL * delta_time
} else if (objectsAreBelow(character)) {
    character.velY = -DOWN_ACCEL * delta_time
} else if (objectsAreBelow(character)) {
    character.velY -= DOWN_ACCEL * delta_time
}

moveObject(character, 0, y)