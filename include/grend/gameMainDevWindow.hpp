#pragma once

#include <grend/gameState.hpp>   // TODO: rename to gameState.h
#include <grend/engine.hpp>      // TODO: rename to renderer.h
#include <grend/game_editor.hpp> // TODO: gameEditor
#include <grend/gameMain.hpp>
#include <grend/gameView.hpp>
#include <grend/playerView.hpp>
#include <grend/timers.hpp>
#include <memory>

namespace grendx {

// development instance with editor, a production instance would only
// need a player view
class gameMainDevWindow : public gameMain {
	public:
		gameMainDevWindow() : gameMain("[grend editor]") {
			player = gameView::ptr(new playerView());
			editor = gameView::ptr(new game_editor(this));
			view   = editor;

			state = game_state::ptr(new game_state());
			rend  = renderer::ptr(new renderer(ctx));
		}
		virtual void handleInput(void);

		gameView::ptr player;
		gameView::ptr editor;
};

// namespace grendx
}