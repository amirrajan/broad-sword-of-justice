#ifndef draw_h
#define draw_h

void game_draw_sprite_or_reset(SDL_Context *context,
			       BSJ_Sprite *sprite,
			       bool draw_if,
			       int x,
			       int y,
			       int w,
			       int h,
			       int facing);
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game);

#endif // draw_h
