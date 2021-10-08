#include "game_object.h"
#include <system/debug_log.h>

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body)
{
	if (body)
	{
	
			// setup object rotation
			gef::Matrix44 object_rotation;
			object_rotation.RotationZ(body->GetAngle());

			// setup the object translation
			gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

			// build object transformation matrix
			gef::Matrix44 object_transform = object_rotation;
			object_transform.SetTranslation(object_translation);
			set_transform(object_transform);
		
	}

	
}

void GameObject::UpdateFromSimulation(const b2Body* body, float player) // overload
{
	if (body)
	{
		

			gef::Matrix44 object_scale;
			object_scale.Scale(gef::Vector4(3.0, 3.0, 0));

			// setup object rotation
			gef::Matrix44 object_rotation;
			object_rotation.RotationZ(gef::DegToRad(player));
			//object_rotation.RotationX(gef::DegToRad(90));

			// setup the object translation
			gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

			// build object transformation matrix
			gef::Matrix44 object_transform = object_rotation;
			object_transform.SetTranslation(object_translation);

			set_transform(object_scale * object_transform);


	}
		
}

void GameObject::MyCollisionResponse()
{
	//gef::DebugOut("A collision has happened.\n");
}

Player::Player()
{
	set_type(PLAYER);
}

void Player::DecrementHealth()
{
	//gef::DebugOut("Player has taken damage.\n");
}


