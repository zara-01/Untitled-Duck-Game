#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/sprite.h>
//#include "load_texture.h"

// constructor 
SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	input_manager_(NULL),
	font_(NULL),
	world_(NULL),
	game_state_(GameState_::Init),
	state_timer(0.0f),
	audio_manager_(NULL),
	selected(0),
	difficulty(0),
	quitOut(false)
{
}

// initialise
void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	InitFont();
	

	// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);

	audio_manager_ = gef::AudioManager::Create();

	FrontendInit();
	//GameInit();
	
	float musicVolume = 70.0f;
	audio_manager_->SetMasterVolume(musicVolume);
	audio_manager_->LoadMusic("pianoloop.wav", platform_);
	audio_manager_->LoadSample("enemy.wav", platform_); //0
	audio_manager_->LoadSample("Chicken_plop.wav", platform_); //1
	audio_manager_->LoadSample("Chicken_hurt1.wav", platform_); //2
	audio_manager_->PlayMusic();

	game_state_ = GameState_::Init;
	state_timer = 0.0f;
}

// delete
void SceneApp::CleanUp()
{
	delete input_manager_;
	input_manager_ = NULL;

	CleanUpFont();

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

// update sceneapp
bool SceneApp::Update(float frame_time) // switch
{
	fps_ = 1.0f / frame_time;

	state_timer += frame_time;

	input_manager_->Update();

	// gamestates
	switch (game_state_)
	{
	case GameState_::Init:
		InitStateUpdate(frame_time);
		break;
	case GameState_::Menu:
		MenuUpdate(frame_time);
		break;

	case GameState_::Level1:
		GameStateUpdate(frame_time);
		break;

	case GameState_::Setting:
		// update settings
		SettingsUpdate(frame_time);
		break;

	case GameState_::OVER:
		// update settings
		OverUpdate(frame_time);
		break;

	case GameState_::Exit:
		
		break; // quit game

	default:
		break;

	}

	if (!quitOut)
	{
		return true;
	}
	else if (quitOut)
	{
		return false;
	}
	
}


// sceneapp render
void SceneApp::Render() /// switch
{

	// state switch
	switch (game_state_)
	{
	case GameState_::Init:
		InitStateRender();
		break;

	case GameState_::Menu:
		MenuRender();
		break;

	case GameState_::Level1:
		GameStateRender();
		break;

	case GameState_::Setting:
		// render settings
		SettingsRender();
		break;

	case GameState_::OVER:
		// update settings
		OverRender();
		break;

	case GameState_::Exit:
		break; // quit game

	default:
		break;

	}
}



// init the ground / base of the level, perhaps could have own class later
void SceneApp::InitGround()
{

	wallOne.initWall(gef::Vector4(35.0f, 1.0f, 0.5f), gef::Vector4(0.f, 26.f, 0.f), world_, sprite_renderer_, renderer_3d_, primitive_builder_);
	wallTwo.initWall(gef::Vector4(1.0f, 27.0f, 0.5f), gef::Vector4(-36.f, 0.0f, 0.f), world_, sprite_renderer_, renderer_3d_, primitive_builder_);
	wallThree.initWall(gef::Vector4(35.0f, 1.0f, 0.5f), gef::Vector4(0.f, -26.f, 0.f), world_, sprite_renderer_, renderer_3d_, primitive_builder_);
	wallFour.initWall(gef::Vector4(1.0f, 27.0f, 0.5f), gef::Vector4(36.f, 0.0f, 0.f), world_, sprite_renderer_, renderer_3d_, primitive_builder_);
	/*wallOne.set_type(WALL);
	wallTwo.set_type(WALL);
	wallThree.set_type(WALL);
	wallFour.set_type(WALL);*/

	// ground dimensions
	gef::Vector4 ground_half_dimensions(35.0f, 25.0f, 0.5f);
	//gef::Vector4 wall_half_dimensions(35.0f, 1.0f, 0.5f);

	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def; // collider information
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 0.0f);


	ground_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());


	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	ground_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	ground_.UpdateFromSimulation(ground_body_);

	gef::Matrix44 translationMatrix;
	translationMatrix.SetIdentity();
	translationMatrix.SetTranslation(gef::Vector4(0, 0, -1));
	ground_.set_transform(ground_.transform() * translationMatrix);


	ground_body_->SetEnabled(false);
	

}

// front end things
void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

// hud
void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xfffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.50f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.9f, 0.9f, 0.9f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
} // lights

// update physics sim
void SceneApp::UpdateSimulation(float frame_time)
{

	// update physics world
	float timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// don't have to update the ground visuals as it is static

	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// COLLISION RESPONSE HERE
			PlayerManager* player = NULL;
			Enemy* enemy = NULL;
			Bullet* bullet = NULL;
			Walls* wall = NULL;

			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = reinterpret_cast<GameObject*>(bodyA->GetUserData().pointer);
			gameObjectB = reinterpret_cast<GameObject*>(bodyB->GetUserData().pointer);

			if (gameObjectA)
			{

				if (gameObjectA->type() == BULLET)
				{
					bullet = reinterpret_cast<Bullet*>(bodyA->GetUserData().pointer);
				}

				if (gameObjectA->type() == ENEMY)
				{
					enemy = reinterpret_cast<Enemy*>(bodyA->GetUserData().pointer);
				}

				if (gameObjectA->type() == PLAYER)
				{
					player = reinterpret_cast<PlayerManager*>(bodyA->GetUserData().pointer);
				}

				if (gameObjectA->type() == WALL)
				{
					wall = reinterpret_cast<Walls*>(bodyA->GetUserData().pointer);
				}

			}

			if (gameObjectB)
			{
				if (gameObjectB->type() == BULLET)
				{
					bullet = reinterpret_cast<Bullet*>(bodyB->GetUserData().pointer);
				}

				if (gameObjectB->type() == ENEMY)
				{
					enemy = reinterpret_cast<Enemy*>(bodyB->GetUserData().pointer);
				}

				if (gameObjectB->type() == PLAYER)
				{
					player = reinterpret_cast<PlayerManager*>(bodyB->GetUserData().pointer);
				}

				if (gameObjectB->type() == WALL)
				{
					wall = reinterpret_cast<Walls*>(bodyB->GetUserData().pointer);
				}

			}

			if (bullet && enemy)
			{
				enemy->setDead();
				audio_manager_->PlaySample(0, false);
				bullet->die();
				audio_manager_->PlaySample(1, false);
				player_one_->incScore();
			}

			if (player && enemy)
			{
				player->decrementLives();
				//player_one_->decrementLives();
				//gef::DebugOut("player coll");
			}

			if (bullet && wall)
			{
				audio_manager_->PlaySample(1, false);
				bullet->die();
			}

			

			
		}

		// Get next contact point
		contact = contact->GetNext();
	}
} //collision

// more front end stuff idk
void SceneApp::FrontendInit()
{
	//button_icon_ = CreateTextureFromPNG("playstation-cross-dark-icon.png", platform_);
	// splash
	loader = modelLoader->CreateTextureFromPNG("loading.png", platform_);
}

void SceneApp::FrontendRelease()
{
	loader = NULL;
}

void SceneApp::FrontendUpdate(float frame_time)
{
	//const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
}

void SceneApp::FrontendRender()
{
	sprite_renderer_->Begin();

	gef::Sprite background;
	background.set_texture(loader);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);


	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::MenuInit()
{

	main_menu = modelLoader->CreateTextureFromPNG("menu.png", platform_);

}

void SceneApp::MenuRelease()
{

	main_menu = NULL;
}

void SceneApp::MenuUpdate(float frame_time)
{

	const gef::Keyboard* keyboard = input_manager_->keyboard();

	if (keyboard)
	{
		if (keyboard->IsKeyPressed(keyboard->KC_W))
		{
			audio_manager_->PlaySample(1, false);

			if (selected > 0)
			{
				selected -= 1;
			}
			else
			{
				selected = 1;
			}
		}

		if (keyboard->IsKeyPressed(keyboard->KC_S))
		{
			audio_manager_->PlaySample(1, false);

			if (selected < 1)
			{
				selected += 1;
			}
			else
			{
				selected = 0;
			}
		}

		if (keyboard->IsKeyPressed(keyboard->KC_SPACE))
		{
			audio_manager_->PlaySample(1, false);

			if (selected == 0)
			{
				ChangeGameState(Setting);
			}
			else if (selected == 1)
			{
				ChangeGameState(Exit);
				
			}
		}
	}

}

void SceneApp::MenuRender()
{

	sprite_renderer_->Begin();

	gef::Sprite background;
	background.set_texture(main_menu);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);

	// render buttons

	if (selected == 0)
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.6f, platform_.height() * 0.5f - 56.0f, -0.99f),

			1.2f,

			0xff0000FF,

			gef::TJ_LEFT,

			"start");
	}
	else
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.6f, platform_.height() * 0.5f - 56.0f, -0.99f),

			1.0f,

			0xffffffff,

			gef::TJ_LEFT,

			"start");
	}

	if (selected == 1)
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.6f, platform_.height() * 0.7f - 56.0f, -0.99f),

			1.2f,

			0xff0000FF,

			gef::TJ_LEFT,

			"quit");
	}
	else
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.6f, platform_.height() * 0.7f - 56.0f, -0.99f),

			1.0f,

			0xffffffff,

			gef::TJ_LEFT,

			"quit");
	}

	//DrawHUD();
	sprite_renderer_->End();

}

void SceneApp::SettingsInit()
{
	difficulty = 0;
	settings = modelLoader->CreateTextureFromPNG("settings.png", platform_);

}

void SceneApp::SettingsRelease()
{

	settings = NULL;

}

void SceneApp::SettingsUpdate(float frame_time)
{

	const gef::Keyboard* keyboard = input_manager_->keyboard();

	if (keyboard)
	{

		if (keyboard->IsKeyPressed(keyboard->KC_W))
		{
			audio_manager_->PlaySample(1, false);

			if (selected > 0)
			{
				selected -= 1;
			}
			else
			{
				selected = 1;
			}
		}

		if (keyboard->IsKeyPressed(keyboard->KC_S))
		{
			audio_manager_->PlaySample(1, false);

			if (selected < 1)
			{
				selected += 1;
			}
			else
			{
				selected = 0;
			}
		}

		if (keyboard->IsKeyPressed(keyboard->KC_SPACE))
		{
			audio_manager_->PlaySample(1, false);

			if (selected == 0)
			{
				//ChangeGameState(Setting);
				// easy difficulty
				difficulty = 1;
				ChangeGameState(Level1);
			}
			else if (selected == 1)
			{
				//ChangeGameState(Exit);
				// hard difficulty
				difficulty = 1;
				ChangeGameState(Level1);

			}
		}
	}

}

void SceneApp::SettingsRender()
{

	sprite_renderer_->Begin();

	gef::Sprite background;
	background.set_texture(settings);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);

	// render buttons

	if (selected == 0)
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.35f - 56.0f, -0.99f),

			1.2f,

			0xff0000FF,

			gef::TJ_LEFT,

			"easy");
	}
	else
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.35f - 56.0f, -0.99f),

			1.0f,

			0xffffffff,

			gef::TJ_LEFT,

			"easy");
	}

	if (selected == 1)
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.45f - 56.0f, -0.99f),

			1.2f,

			0xff0000FF,

			gef::TJ_LEFT,

			"hard");
	}
	else
	{
		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.45f - 56.0f, -0.99f),

			1.0f,

			0xffffffff,

			gef::TJ_LEFT,

			"hard");
	}

	//DrawHUD();
	sprite_renderer_->End();

}

void SceneApp::OverInit()
{
	//get player score
	endscreen = modelLoader->CreateTextureFromPNG("gameover.png", platform_);

}

void SceneApp::OverRelease()
{
	endscreen = NULL;
}

void SceneApp::OverUpdate(float frame_time)
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();

	if (keyboard)
	{
		if (keyboard->IsKeyPressed(keyboard->KC_SPACE))
		{
				//ChangeGameState(Setting);
				// easy difficulty
				difficulty = 1;
				ChangeGameState(Menu);
		}
	}
}

void SceneApp::OverRender()
{
	sprite_renderer_->Begin();

	gef::Sprite background;
	background.set_texture(endscreen);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);

	// render buttons

	std::string str;
	str.append("Final Score: ");
	str.append(std::to_string(finalScore)); // gets the score as int)
	char const* pchar = str.c_str(); // casts it to a const char

		font_->RenderText(
			sprite_renderer_,

			gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.45f - 56.0f, -0.99f),

			1.2f,

			0xff0000FF,

			gef::TJ_LEFT,

			pchar);
	

	
	//DrawHUD();
	sprite_renderer_->End();


}

// game init
void SceneApp::GameInit()
{
	gef::Colour colour_;
	colour_.SetFromRGBA(0x0c0c0a00);
	platform_.set_render_target_clear_colour(colour_);


	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	SetupLights();

	// initialise the physics world
	b2Vec2 gravity(0.0f, 0.0f);
	world_ = new b2World(gravity);

	//InitPlayer();
	//player_one_->InitPlayer();
	player_one_ = new PlayerManager(primitive_builder_, world_, renderer_3d_, &platform_);
	player_one_->InitPlayer();

	// initialise enemy manager
	enemy_manager_ = new EnemyManager(world_, sprite_renderer_, renderer_3d_, primitive_builder_, &platform_, difficulty);
	enemy_manager_->InitEnemies();

	playerBullets_ = new BulletManager(world_, sprite_renderer_, renderer_3d_, primitive_builder_);
	playerBullets_->InitBullets();


	InitGround();
	initOcean();
}

// delete
void SceneApp::GameRelease()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete player_one_;
	delete playerBullets_;
	delete enemy_manager_;

	//modelLoader = NULL;


}

// game update
void SceneApp::GameUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	//audio_manager_->PlayMusic();

	if (!player_one_->playerStatus()) // while the player is still alive
	{

		//input_manager_->keyboard()->Update();
		playerBullets_->CreateNew(input_manager_, player_one_->player_body_->GetPosition(), frame_time);

		player_one_->MovePlayer(frame_time, input_manager_); //update physics

		enemy_manager_->CreateNew(frame_time);

		UpdateSimulation(frame_time); // UPDATES PHYSICS

		//update player sim
		player_one_->Update(frame_time);

		if (!player_one_->playerStatus()) // while the player is still alive
		{
			// update enemies
			enemy_manager_->Update(frame_time, player_one_->player_body_->GetPosition());
		}
		
		// update bullets
		playerBullets_->Update(frame_time);
	}
	else if (player_one_->playerStatus())
	{
		finalScore = player_one_->getScore();
		ChangeGameState(OVER);
	}
	

}

// game render, camera etc
void SceneApp::GameRender()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix); 


	// view
	//gef::Vector4 camera_eye(0.0f, -10.0f, 70.0f); // pos of the camera in space (frustrum <3)
	gef::Vector4 camera_eye(0.0f + player_one_->player_body_->GetPosition().x, -10.0f + player_one_->player_body_->GetPosition().y, 50.0f); // pos of the camera in space (frustrum <3)
	gef::Vector4 camera_lookat(player_one_->player_body_->GetPosition().x, player_one_->player_body_->GetPosition().y, 0.0f); // what the camera is positioned to look at 
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f); // up vector <3333
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// draw 3d geometry
	renderer_3d_->Begin();

	//renderer_3d_->set_override_material(mat);
	renderer_3d_->DrawMesh(mesh_instance_);

	// draw ground
	renderer_3d_->set_override_material(pondtex);
	renderer_3d_->DrawMesh(ground_);
	renderer_3d_->set_override_material(NULL);


	// draw wall
	//renderer_3d_->DrawMesh(wall_);
	wallOne.renderWall();
	wallTwo.renderWall();
	wallThree.renderWall();
	wallFour.renderWall();

	//// draw player
	
	player_one_->Render();

	// draw Enemies
	enemy_manager_->Render();

	// draw bullets 
	playerBullets_->Render();

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

// model loading

//backgriound pond // initially waves but changed to pond model
void SceneApp::initOcean()
{
	// load the assets in from the .scn
	const char* scene_asset_filename = "pond.scn";
	scene_assets_ = modelLoader->LoadSceneAssets(platform_, scene_asset_filename);
	if (scene_assets_)
	{
		mesh_instance_.set_mesh(modelLoader->GetMeshFromSceneAssets(scene_assets_));
	}
	else
	{
		gef::DebugOut("Scene file %s failed to load\n", scene_asset_filename);
	}

	
	gef::Matrix44 rotation_;
	rotation_.SetIdentity();
	rotation_.RotationX(gef::DegToRad(90));
	//mesh_instance_.set_transform(mesh_instance_.transform() * rotation_);


	gef::Matrix44 translate_;
	translate_.SetIdentity();
	translate_.Scale(gef::Vector4(50, 50, 0));
	translate_.SetTranslation(gef::Vector4(0, 0, -50));

	mesh_instance_.set_transform(rotation_ * translate_);

	gef::Texture* texture;
	texture = modelLoader->CreateTextureFromPNG("pixelwatertrans.png", platform_);

	pondtex = new gef::Material();
	//pondtex->set_colour();
	pondtex->set_texture(texture);

	
	
}


///////////////////////////////////////////////////// game states //////////////////////////////////////////////////////


void SceneApp::InitStateUpdate(float frame_time)
{
	
	// splash screen

	if (state_timer > 4.0f)
	{
		ChangeGameState(GameState_::Menu);
	}
}

void SceneApp::InitStateRender()
{
	
	// splash
	FrontendRender();

}

//void SceneApp::MenuStateUpdate(float frame_time)
//{
//	//update menu
//
//}
//
//void SceneApp::MenuStateRender()
//{
//
//	// render menu
//
//	sprite_renderer_->Begin();
//
//	// render "TO START" text
//	font_->RenderText(
//		sprite_renderer_,
//		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f + 32.0f, -0.99f),
//		1.0f,
//		0xffffffff,
//		gef::TJ_CENTRE,
//		"MENU");
//
//
//	DrawHUD();
//	sprite_renderer_->End();
//}

void SceneApp::GameStateInit()
{

	GameInit();

}

void SceneApp::GameStateUpdate(float frame_time)
{

	GameUpdate(frame_time);

}

void SceneApp::GameStateRender()
{
	sprite_renderer_->Begin();

	renderer_3d_->Begin();

	GameRender();

	std::string str;
	str.append("Score: ");
	str.append(std::to_string(player_one_->getScore())); // gets the score as int)
	char const* pchar = str.c_str(); // casts it to a const char

	std::string Lstr;
	Lstr.append("Lives Left: ");
	Lstr.append(std::to_string(player_one_->playerLiveCount())); // gets the score as int)
	char const* Lchar = Lstr.c_str(); // casts it to a const char

	// render "life and score " text
	font_->RenderText(
		sprite_renderer_,

		gef::Vector4(platform_.width() * 0.02f, platform_.height() * 0.04f + 32.0f, -0.99f),

		1.0f,

		0xff0000FF,

		gef::TJ_LEFT,

		pchar);

	font_->RenderText(
		sprite_renderer_,

		gef::Vector4(platform_.width() * 0.02f, platform_.height() * 0.08f + 32.0f, -0.99f),

		1.0f,

		0xff0000FF,

		gef::TJ_LEFT,

		Lchar);




	DrawHUD();

	renderer_3d_->End();

	sprite_renderer_->End();


}

void SceneApp::GameStateRelease()
{
	GameRelease();
}

void SceneApp::ChangeGameState(GameState_ new_state)
{

	switch (game_state_)
	{
	case  GameState_::Init: // cleanupinit
		FrontendRelease();
		break;

	case  GameState_::Menu:
		MenuRelease();
		break; // cleanup menu

	case  GameState_::Level1:
		GameRelease();
		break; //cleanupmenu

	case GameState_::Setting:
		SettingsRelease();
		break; // cleanup menu

	default:
		break;
	}

	switch (new_state)
	{
	case  GameState_::Init: 

		FrontendInit();
		// set up init
		break;

	case  GameState_::Menu:
		MenuInit();
		//FrontendInit();
		game_state_ = Menu;
		state_timer = 0;
		break; // cleanup menu

	case  GameState_::Level1:

		GameInit();
		game_state_ = Level1;
		state_timer = 0;

		break; //cleanuplevel

	case GameState_::Setting:

		SettingsInit();
		game_state_ = Setting;
		state_timer = 0;
		break; // cleanup menu

	case GameState_::OVER:

		OverInit();
		game_state_ = OVER;
		state_timer = 0;
		break; // cleanup menu

	case GameState_::Exit:
		quitOut = true;
		break;

	default:
		break;
	}

	//clean up old state
	//game_state_ = new_state;
	//state_timer = 0;

	// init new state
}
