#include "SDLWidgetRenderer.h"
#include "UI/Visual/text_visual.h"
#include "UI/Visual/colour_visual.h"
#include "UI/Visual/image_visual.h"
#include "UI/Core/text_style.h"
#include "Misc/debug.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <math.h>

namespace Retro3D
{
	void SDLWidgetRenderer::RenderColour(ColourVisual* arg_visual, const WidgetRenderParams& arg_renderparams)
	{
		const glm::vec4& rgba = arg_visual->GetColour().GetRGBA();
		int window_width;
		int window_height;
		SDL_GetWindowSize(mWindow->GetSDLWindow(), &window_width, &window_height);

		SDL_Renderer* renderer = mWindow->GetSDLRenderer();
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, rgba.r * 255, rgba.g * 255, rgba.b * 255, rgba.a * 255);
		SDL_Rect rect;
		rect.x = arg_renderparams.mVisibleRect.mPosition.x * window_width;
		rect.y = arg_renderparams.mVisibleRect.mPosition.y * window_height;
		rect.w = arg_renderparams.mVisibleRect.mSize.x * window_width;
		rect.h = arg_renderparams.mVisibleRect.mSize.y * window_height;
		SDL_RenderFillRect(renderer, &rect);
	}

	void SDLWidgetRenderer::RenderImage(ImageVisual* arg_image, const WidgetRenderParams& arg_renderparams)
	{
		SDL_Renderer* renderer = mWindow->GetSDLRenderer();

		int window_width;
		int window_height;
		SDL_GetWindowSize(mWindow->GetSDLWindow(), &window_width, &window_height);

		SDL_Texture* img = arg_image->GetSDLTexture();
		if (img == nullptr)
		{
			LOG_ERROR() << "Image visual has no loaded texture: " << arg_image->GetImagePath();
			return;
		}
		int w, h;
		SDL_QueryTexture(img, NULL, NULL, &w, &h);

		const glm::vec2 relPos = (arg_renderparams.mVisibleRect.mPosition - arg_renderparams.mContentRect.mPosition) / arg_renderparams.mContentRect.mSize;
		const glm::vec2 srcPos = glm::vec2(w * relPos.x, h * relPos.y);
		const glm::vec2 relSize = arg_renderparams.mVisibleRect.mSize / arg_renderparams.mContentRect.mSize;
		const glm::vec2 srcSize = glm::vec2(w * relSize.x, h * relSize.y);

		SDL_Rect srcRect;
		srcRect.x = relPos.x * w;
		srcRect.y = relPos.y * h;
		srcRect.w = relSize.x * w;
		srcRect.h = relSize.y * h;

		SDL_Rect dstRect;
		dstRect.x = arg_renderparams.mVisibleRect.mPosition.x * window_width;
		dstRect.y = arg_renderparams.mVisibleRect.mPosition.y * window_height;
		dstRect.w = arg_renderparams.mVisibleRect.mSize.x * window_width;
		dstRect.h = arg_renderparams.mVisibleRect.mSize.y * window_height;

		SDL_RenderCopy(renderer, img, &srcRect, &dstRect);
	}


	void SDLWidgetRenderer::RenderText(TextVisual* arg_text, const WidgetRenderParams& arg_renderparams)
	{
		const TextStyle& style = arg_text->GetTextStyle();
		const bool wrapText = style.GetWrapText();
		const std::string& text = arg_text->GetText();

		int window_width;
		int window_height;
		SDL_GetWindowSize(mWindow->GetSDLWindow(), &window_width, &window_height);

		SDL_Renderer* renderer = mWindow->GetSDLRenderer();
		
		SDL_Rect rect;
		rect.x = arg_renderparams.mVisibleRect.mPosition.x * window_width;
		rect.y = arg_renderparams.mVisibleRect.mPosition.y * window_height;
		rect.w = arg_renderparams.mVisibleRect.mSize.x * window_width;
		rect.h = arg_renderparams.mVisibleRect.mSize.y * window_height;

		TTF_Font* font = arg_text->GetFontRes()->GetFont();

		if (font == nullptr)
		{
			LOG_ERROR() << "Failed to open font for text: " << arg_text->GetText().c_str();
			return;
		}

		SDL_Color White = { 255, 255, 255 };

		SDL_Surface* surface;
		if(wrapText)
			surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), White, (Uint32)rect.w);
		else
			surface = TTF_RenderText_Solid(font, text.c_str(), White);

		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == nullptr)
		{
			LOG_ERROR() << "Failed to create texture";
			return;
		}

		const int iW = surface->w;
		const int iH = surface->h;
		//SDL_QueryTexture(texture, NULL, NULL, &iW, &iH);
		/*
		if (iW < rect.w)
		{
			rect.x += (rect.w - iW) / 2;
			rect.w = iW;
		}
		if (iH < rect.h)
		{
			rect.y += (rect.h - iH) / 2;
			rect.h = iH;
		}
		*/
		SDL_Texture* txtTexture = SDL_CreateTextureFromSurface(renderer, surface);

		const glm::vec2 relPos = (arg_renderparams.mVisibleRect.mPosition - arg_renderparams.mContentRect.mPosition) / arg_renderparams.mContentRect.mSize;
		const glm::vec2 srcPos = glm::vec2(iW * relPos.x, iH * relPos.y);
		const glm::vec2 relSize = arg_renderparams.mVisibleRect.mSize / arg_renderparams.mContentRect.mSize;
		const glm::vec2 srcSize = glm::vec2(iW * relSize.x, iH * relSize.y);

		SDL_Rect srcRect;
		srcRect.x = relPos.x * iW;
		srcRect.y = relPos.y * iH;
		srcRect.w = relSize.x * iW;
		srcRect.h = relSize.y * iH;

		SDL_RenderCopy(renderer, txtTexture, &srcRect, &rect);

		SDL_FreeSurface(surface); // TODO: use resource manager
		SDL_DestroyTexture(txtTexture); // TODO: use resource manager
	}
	
	void SDLWidgetRenderer::SetWindow(Window* arg_window)
	{
		mWindow = arg_window;
	}
	
	
	void SDLWidgetRenderer::InitTransform(const Rectangle& arg_contentrect, const Rectangle& arg_visiblerect, SDL_Rect& out_rect)
	{
		int window_width;
		int window_height;
		SDL_GetWindowSize(mWindow->GetSDLWindow(), &window_width, &window_height);

		SDL_Renderer* renderer = mWindow->GetSDLRenderer();
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		out_rect.x = arg_contentrect.mPosition.x * window_width;
		out_rect.y = arg_contentrect.mPosition.y * window_height;
		out_rect.w = arg_contentrect.mSize.x * window_width;
		out_rect.h = arg_contentrect.mSize.y * window_height;
	}

}