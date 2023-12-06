// Uses Object0 as hopefully there will always be an Object0
// to find the Object guid that is listed in the static curr_guid of Object
var object = gethandle(Object0.curr_guid) 
// Then do stuff with said object
print("Received new object: " +  object.guid)