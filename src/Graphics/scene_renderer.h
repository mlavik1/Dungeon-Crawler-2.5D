#ifndef RETRO3D_SCENERENDERER_H
#define RETRO3D_SCENERENDERER_H

/*=============================================================================================
Scene Renderer

Renders walls, ceilings, floors and sprites.

Everything is software rendered (obviously for educational purposes, as it is super-inefficient).

==============================================================================================*/

#include <stdint.h>
#include <unordered_map>
#include "Object/objectptr.h"
#include "texture.h"
#include <glm/glm.hpp>

struct SDL_Surface;
struct SDL_Texture;

namespace Retro3D
{
	class Level;
	class CameraComponent; // TODO: This is bad!

	class SceneRenderer
	{
	public:
		SceneRenderer();
		~SceneRenderer();

		/** Renders the whole scene. */
		void RenderScene();

		/** Sets the Level to render. Called from the engine. */
		void SetLevel(Level* arg_level);

		void SetCameraComponent(CameraComponent* arg_comp); // TEMP

	private:
		Level* mLevel;
		std::unordered_map<char, SDL_Surface*> mTextureSurfaceMap; // TODO: use array instead?
		SDL_Surface* mSkyboxTexture = nullptr;
		ObjectPtr<CameraComponent> mCameraComponent; // TODO: pass on some other structure
		float mFOV = 95;
		SDL_Texture* mRenderTexture;
		std::vector<unsigned char> mPixels;
		std::vector<unsigned char> mClearPixels;
		std::vector<float> mDepthBuffer;
		std::vector<float> mClearDepthBuffer;
	};

	class SpriteRenderObject
	{
	public:
		const glm::vec3 mPosition;
		const Texture* mTexture;
		SpriteRenderObject(const glm::vec3& arg_pos, const Retro3D::Texture* arg_texture)
			: mPosition(arg_pos), mTexture(arg_texture) 
		{ }
	};
}

#endif
