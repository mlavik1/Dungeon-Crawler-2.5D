#ifndef RETRO3D_GAMEENGINE_H
#define RETRO3D_GAMEENGINE_H

/*=============================================================================================
Game Engine

The core of the game egine.
Contains:
	- All managers (InputManager, ScriptManager)
	- World (where objects exist)
	and more.
The game is ticked from here.
==============================================================================================*/

#include "Object/objectptr.h"
#include "Text/config_reader.h"

namespace Retro3D
{
	// Forward declarations
	class Level;
	class Window;
	class InputManager;
	class ScriptManager;
	class WidgetManager;
	class SceneRenderer;
	class ResourceManager;
	class World;
	class PlayerController;
	class WorldMessageBus;
	class IWidgetRenderer;

	class GameEngine
	{
	private:
		World* mWorld;
		WorldMessageBus* mWorldMessageBus;
		Level* mCurrentLevel;
		Window* mWindow;

		// Managers
		InputManager* mInputManager;
		ScriptManager* mScriptManager;
		WidgetManager* mWidgetManager;
		SceneRenderer* mSceneRenderer;
		ResourceManager* mResourceManager;
		
		PlayerController* mPlayerController;
		ConfigReader mGameConfig;
		IWidgetRenderer* mWidgetRenderer;

		float mDeltaTime = 0.1f;
		bool mIsRunning;

		// Constructor
		GameEngine();

	public:
		/**
		* Creates the GameEngine.
		* This will set the global GGameEngine instance.
		* @return A pointer to the newly created GGameEngine
		*/
		static GameEngine* CreateGameEngine();

	public:
		/** Starts the game engine */
		void StartEngine();

		/** Called every frame */
		void TickEngine();

		void Shutdown();

		/** Gets the time in seconds between current and last frame */
		float GetDeltaTime();

		void SetPlayerController(PlayerController* arg_controller) { mPlayerController = arg_controller; } // TODO

		// Accessors
		inline Level* GetCurrentLevel() { return mCurrentLevel; }
		inline Window* GetWindow() { return mWindow; }
		inline InputManager* GetInputManager() { return mInputManager; }
		inline ScriptManager* GetScriptManager() { return mScriptManager; }
		inline WidgetManager* GetWidgetManager() { return mWidgetManager; }
		inline SceneRenderer* GetSceneRenderer() { return mSceneRenderer; }
		inline ResourceManager* GetResourceManager() { return mResourceManager; }
		inline World* GetWorld() { return mWorld; }
		inline PlayerController* GetPlayerController() { return mPlayerController; }
		inline ConfigReader& GetGameConfig() { return mGameConfig; }
		inline WorldMessageBus* GetWorldMessageBus() { return mWorldMessageBus; }
		inline IWidgetRenderer* GetWidgetRenderer() { return mWidgetRenderer; }
	};
}

namespace Retro3D
{
	extern GameEngine* GGameEngine;
}

#endif
