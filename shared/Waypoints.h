/* Waypoints — C port of shared/Waypoints.pas */

#ifndef WAYPOINTS_H
#define WAYPOINTS_H

#include <stdbool.h>

#define MAX_WAYPOINTS 5000
#define MAX_CONNECTIONS 20

/* TWaypointAction enum */
#define WAYPOINTACTION_NONE 0
#define WAYPOINTACTION_STOPANDCAMP 1
#define WAYPOINTACTION_WAIT1SECOND 2
#define WAYPOINTACTION_WAIT5SECONDS 3
#define WAYPOINTACTION_WAIT10SECONDS 4
#define WAYPOINTACTION_WAIT15SECONDS 5
#define WAYPOINTACTION_WAIT20SECONDS 6

typedef struct TWaypoint {
    bool Active;
    int Id;
    int X, Y;
    bool Left, Right, Up, Down, Jetpack;
    unsigned char PathNum;
    int Action;
    int ConnectionsNum;
    int Connections[MAX_CONNECTIONS + 1]; /* Pascal 1..MAX_CONNECTIONS */
} TWaypoint;

typedef struct TWaypoints {
    TWaypoint Waypoint[MAX_WAYPOINTS + 1]; /* Pascal 1..MAX_WAYPOINTS */
} TWaypoints;

int TWaypoints_FindClosest(TWaypoints *self, float x, float y, int radius,
    int currWaypoint);

#endif /* WAYPOINTS_H */