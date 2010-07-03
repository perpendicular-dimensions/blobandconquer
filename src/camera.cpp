#include "camera.h"

void moveCamera()
{
	checkEntityToWorld(camera);
	
	camera->position = tracer->outputEnd;
	camera->contentType = tracer->outputContent;
}

void moveCameraToOriginalPosition()
{
	camera->thinkTime -= engine->getTimeDifference(TD_LOGIC);
			
	if (camera->thinkTime < 100)
	{
		/* Smoothly move back to regular view */
		
		float hSpeed = (3 * engine->getTimeDifference(TD_LOGIC));
		float rSpeed = (2 * engine->getTimeDifference(TD_LOGIC));
		
		if (game->cameraTempHeight < game->cameraHeight)
		{
			game->cameraTempHeight += hSpeed;
		}
		else
		{
			game->cameraTempHeight -= hSpeed;
		}
		
		if (fabs(game->cameraTempHeight - game->cameraHeight) <= hSpeed)
		{
			game->cameraTempHeight = game->cameraHeight;
		}
		
		game->cameraRadius += rSpeed;
		
		if (game->cameraRadius > game->desiredCameraRadius)
		{
			game->cameraRadius = game->desiredCameraRadius;
		}
		
		/* set the thinktime to 0 if you're correctly aligned */
		
		if ((game->cameraTempHeight == game->cameraHeight) && (game->desiredCameraRadius == game->cameraRadius))
		{
			camera->thinkTime = 0;
			
			game->cameraTempHeight = game->cameraHeight;
			game->cameraRadius = game->desiredCameraRadius;
		}
	}
}

void cameraFinalizePosition()
{
	/*
	* We need to compensate for the fact that the angle the camera is at might be completely
	* different to the angle the player is moving in. ie - Ensure pushing forward always moves
	* the player up the screen and not in another direction.
	*/
	float dx = player->position.x - camera->position.x;
	float dy = player->position.y - camera->position.y;
	
	game->cameraAdjustedRadian = atan2(dy, dx);
	game->cameraAdjustedRadian = -90 + toDegrees(game->cameraAdjustedRadian);
	game->cameraAdjustedRadian = toRadians(game->cameraAdjustedRadian);
	
	camera->target = player->position;
	
	if (!game->autoLockTarget)
	{
		camera->target.x += 5;
		camera->target.z += 10;
		camera->target.z += (game->cameraAimHeight / 10);
	}
	
	bbManager->addBox(camera);
}

void handleTightPassage() /*Fnar fnar */
{
	float distance = 0;
	int contents = camera->contentType;
	float origAngle = game->cameraDirRadian;
	float origHeight = game->cameraHeight;
	Vector idealPosition = camera->position;
	
	// try at ground level first
	
	camera->position = player->position;
		
	game->cameraHeight = 10;
	
	camera->velocity.set(game->cameraDir.x, -game->cameraDir.y, game->cameraHeight);
	
	moveCamera();
	
	if (Math::getDistance(player->position.x, player->position.y, tracer->outputEnd.x, tracer->outputEnd.y) >= MIN_CAMERA_DISTANCE)
	{
		return;
	}
	
	game->cameraHeight = origHeight;
	
	/* Try a bunch of angles */

	for (int i = 0 ; i <= 8 && distance < MIN_CAMERA_DISTANCE ; i++)
	{
		camera->position = player->position;
		
		game->changeCameraRotation(45 * PI / 180);
		
		camera->velocity.set(game->cameraDir.x, -game->cameraDir.y, (camera->thinkTime > 0 ? game->cameraTempHeight : 10));
		
		moveCamera();
		
		distance = Math::getDistance(player->position.x, player->position.y, tracer->outputEnd.x, tracer->outputEnd.y);
	}
	
	/* If we still can't get a decent angle, try above the player */
	
	if (distance < MIN_CAMERA_DISTANCE)
	{
		game->cameraDirRadian = 0;
		camera->contentType = contents;
		camera->position = player->position;
		
		camera->velocity.set(game->cameraDir.x, -game->cameraDir.y, 200);
		
		moveCamera();
	}
	
	else
	{
		/* Oh well, welcome to Ninja Gaiden II mode ;) */
		
		game->cameraDirRadian = origAngle;
		camera->position = idealPosition;
		camera->contentType = contents;
	}
}

void handleNormalCamera()
{
	float distance = 0;

	camera->position = player->position;
	
	/* If the player is riding a lift, and it's moving up, then place the camera above the action */
	
	if (player->riding != NULL && (player->riding->entityType == ET_AUTOLIFT || player->riding->entityType == ET_LIFT) && player->riding->velocity.z != 0)
	{
		Structure *s = (Structure *)player->riding;
		
		if (Math::getDistance(s->startPosition, s->endPosition) > 300)
		{
			camera->thinkTime += engine->getTimeDifference(TD_LOGIC);
			
			if (camera->thinkTime >= 50 || player->riding->velocity.z < 0)
			{
				//camera->thinkTime = player->riding->velocity.z ? 250 : 200;
				camera->thinkTime = 165;
			
				game->cameraRadius = 60;
				
				game->cameraTempHeight = 200;
			}
		}
	}
	
	/* Otherwise, just act normally */
	
	else
	{
		/* Deduct thinktime, this is for the Camera on the lift */
		
		if (camera->thinkTime != 0)
		{
			moveCameraToOriginalPosition();
		}
	}
	
	if (camera->thinkTime > 0)
	{
		camera->velocity.set(game->cameraDir.x, -game->cameraDir.y, game->cameraTempHeight);
	}
	else
	{
		camera->velocity.set(game->cameraDir.x, -game->cameraDir.y, game->cameraHeight);
	}
	
	moveCamera();
	
	if (camera->thinkTime == 0)
	{
		distance = Math::getDistance(player->position.x, player->position.y, tracer->outputEnd.x, tracer->outputEnd.y);
		
		if (distance < game->desiredCameraRadius)
		{
			float adjust = game->cameraHeight / game->desiredCameraRadius;
			
			camera->velocity.set(0, 0, -(game->cameraHeight - (adjust * distance)));
			
			moveCamera();
			
			distance = Math::getDistance(player->position.x, player->position.y, tracer->outputEnd.x, tracer->outputEnd.y);
		}
	}
	
	if (camera->thinkTime == 0 && distance < MIN_CAMERA_DISTANCE)
	{
		handleTightPassage();
	}
	
	if (!bbManager->canSightTarget(player, camera))
	{
		if (bbManager->intersectedEntity->entityType == ET_DOOR || bbManager->intersectedEntity->entityType == ET_WEAK_WALL)
		{
			Vector v = player->position;
			
			v.x -= camera->position.x;
			v.y -= camera->position.y;
			v.z -= camera->position.z;
			
			v.x *= bbManager->rayPercentage;
			v.y *= bbManager->rayPercentage;
			v.z *= bbManager->rayPercentage;
			
			camera->position = player->position;
			
			camera->position.x -= v.x;
			camera->position.y -= v.y;
			camera->position.z -= v.z;
			
			if (bbManager->intersectedEntity->touch != NULL)
			{
				//bbManager->intersectedEntity->touch(camera);
			}
		}
	}
	
	cameraFinalizePosition();
}

void handleCutsceneCamera()
{
	camera->position = camera->targetCamera->position;
	camera->target = camera->targetCamera->destination;
}

void doCamera()
{
	tracer->usingCamera = true;
	
	if (game->missionStatus != MISSION_PLAYER_OUT)
	{
		bbManager->removeBox(camera);
		
		if ((camera->targetCamera == NULL) && (game->cutsceneType == CUTSCENE_NONE))
		{
			handleNormalCamera();
		}
		else
		{
			handleCutsceneCamera();
		}
	}
	
	if (camera->shake)
	{
		camera->target.x += Math::rrand(-3, 3);
		camera->target.y += Math::rrand(-3, 3);
	}
	
	camera->use();
	
	tracer->usingCamera = false;
}
