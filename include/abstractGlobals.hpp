#pragma once

#include "main.h"

extern std::string alliance;
extern std::string field_status;
extern bool intakeLock;

extern bool colourSortToggle;
extern bool antiJamToggle;

extern void competitionTelemtryRefresh();
extern bool isResetting;

extern bool bucketLock;

/* Functions */
// DT
void arcadeDrive();

void refreshIntake();
void refreshClamp();
void refreshDoinker();
void refreshWallstakes();
void resetWallstakes();

void liftControl(double target, int minSpeed = 0);

void initializeColourSort();
void driverSkillsMacro();

/* Autons */
