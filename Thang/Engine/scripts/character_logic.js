var DOWN_ACCEL = 0.3
var TERM_VEL = -6


// Uses Object0 as hopefully there will always be an Object0
// to find the Object guid that is listed in the static curr_guid of Object
var character = gethandle(Object0.curr_guid) 

//  This is the logic for non-input based movement


if (areObjectsAbove(character)) {
    character.velY = -DOWN_ACCEL * delta_time
} else if (areObjectsBelow(character)) {
    character.velY = -DOWN_ACCEL * delta_time
} else if (character.velY > TERM_VEL) {
    character.velY -= DOWN_ACCEL * delta_time
}

moveObject(character)