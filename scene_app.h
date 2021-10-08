#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "BulletManager.h"
#include "Walls.h"
#include <graphics/scene.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <cstdlib>
#include "ModelLoading.h"
#include <audio/audio_manager.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
	class Mesh;
	class Scene;
}

enum GameState_ {
	Init,
	Menu,
	Level1,
	Exit,
	Setting,
	OVER
};

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	//void InitPlayer();
	void InitGround();
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void UpdateSimulation(float frame_time);
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_manager_;

	gef::AudioManager* audio_manager_;
	//int bullet, die, hurt;
	

	//
	// FRONTEND DECLARATIONS
	//
	//gef::Texture* button_icon_;

	//
	// GAME DECLARATIONS
	//
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;

	// create the physics world
	b2World* world_;

	// player variables
//	Player player_;
//	b2Body* player_body_;

	PlayerManager* player_one_;

	// Enemy Manager
	EnemyManager* enemy_manager_;

	// bullet mananger

	BulletManager* playerBullets_;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;

	GameObject water;

	// scene

	gef::MeshInstance mesh_instance_;
	gef::Scene* scene_assets_;

	// model loader
	ModelLoading* modelLoader;

	// wall borders
	Walls wallOne;
	Walls wallTwo;
	Walls wallThree;
	Walls wallFour;

	// audio variables
	int sfx_id_;
	int sfx_voice_id_;

	float fps_;

	GameState_ game_state_;
	float state_timer;

	void FrontendInit();
	void FrontendRelease();
	void FrontendUpdate(float frame_time);
	void FrontendRender();

	void MenuInit();
	void MenuRelease();
	void MenuUpdate(float frame_time);
	void MenuRender();

	void SettingsInit();
	void SettingsRelease();
	void SettingsUpdate(float frame_time);
	void SettingsRender();

	void OverInit();
	void OverRelease();
	void OverUpdate(float frame_time);
	void OverRender();

	gef::Texture* main_menu;
	gef::Texture* loader;
	gef::Texture* settings;
	gef::Texture* endscreen;

	int selected;

	void GameInit();
	void GameRelease();
	void GameUpdate(float frame_time);
	void GameRender();

	void initOcean();

	/*gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);

	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);

	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);*/

	//gef::Material* mat; 
	gef::Material* pondtex;


	void InitStateUpdate(float frame_time);
	void InitStateRender();
	
	void GameStateInit();
	void GameStateUpdate(float frame_time);
	void GameStateRender();
	void GameStateRelease();

	int difficulty;
	int finalScore;

	void ChangeGameState(GameState_ new_state);

	bool quitOut;

};

#endif // _SCENE_APP_H
