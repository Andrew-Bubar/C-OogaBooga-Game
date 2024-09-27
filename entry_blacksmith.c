
#include "range.c"
#include "entities.h"

bool almost_equals(float a, float b, float epsilon) {
 return fabs(a - b) <= epsilon;
}

bool animate_f32_to_target(float* value, float target, float delta_t, float rate) {
	*value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
	if (almost_equals(*value, target, 0.001f))
	{
		*value = target;
		return true; // reached
	}
	return false;
}

void animate_v2_to_target(Vector2* value, Vector2 target, float delta_t, float rate) {
	animate_f32_to_target(&(value->x), target.x, delta_t, rate);
	animate_f32_to_target(&(value->y), target.y, delta_t, rate);
}

Vector2 screen_to_world() {
	float mouse_x = input_frame.mouse_x;
	float mouse_y = input_frame.mouse_y;
	Matrix4 proj = draw_frame.projection;
	Matrix4 view = draw_frame.view;
	float window_w = window.width;
	float window_h = window.height;

	// Normalize the mouse coordinates
	float ndc_x = (mouse_x / (window_w * 0.5f)) - 1.0f;
	float ndc_y = (mouse_y / (window_h * 0.5f)) - 1.0f;

	// Transform to world coordinates
	Vector4 world_pos = v4(ndc_x, ndc_y, 0, 1);
	world_pos = m4_transform(m4_inverse(proj), world_pos);
	world_pos = m4_transform(view, world_pos);

	Matrix4 inproj = m4_inverse(proj);
	Matrix4 inview = m4_inverse(view);
/*
	Vector4 ndc4 = v4(ndc_x, ndc_y, 0, 1);
	Vector4 clip_pos = m4_transform(inproj, ndc4);
	Vector4 world_pos = m4_transform(inview, clip_pos);
*/
	//log("%f, %f", world_pos.x, world_pos.y);

	// Return as 2D vector
	return (Vector2){ world_pos.x, world_pos.y };
}

const int tile_size = 8;
const float entity_selection_radious = 16; //radious for detection	
int world_to_tile_pos(float world_pos){
	return roundf(world_pos / (float)tile_size);
}
float tile_pos_to_world_pos(int tile_pos){
	return((float)tile_pos * (float)tile_size);
}
Vector2 round_v2_to_tile(Vector2 world_pos){
	world_pos.x = tile_pos_to_world_pos(world_to_tile_pos(world_pos.x));
	world_pos.y = tile_pos_to_world_pos(world_to_tile_pos(world_pos.y));
	return world_pos;
}

int entry(int argc, char **argv) {
	
	// This is how we (optionally) configure the window.
	// To see all the settable window properties, ctrl+f "struct Os_Window" in os_interface.c
	window.title = STR("Blacksmith Adventures");
	//0x306082ff (demo bg)
	//window.clear_color = COLOR_BLACK;
	window.clear_color = hex_to_rgba(0x836953ff);
	window.width = 1280;
	window.height = 720;

	//world start
	world = alloc(get_heap_allocator(), sizeof(World));

	sprites[SPRITE_PLAYER] = (sprite){
		.image=load_image_from_disk(STR("player.png"), get_heap_allocator()), 
		.size= v2(8, 8)
	};
	sprites[SPRITE_ROCK0] = (sprite){
		.image=load_image_from_disk(STR("rock0.png"), get_heap_allocator()),
		.size= v2(8, 8)
	};
	sprites[SPRITE_TREE0] = (sprite){
		.image=load_image_from_disk(STR("tree0.png"), get_heap_allocator()),
		.size= v2(12, 16)
	};

	//text rendering
	Gfx_Font *font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());
	assert(font, "Failed loading arial.ttf");
	const u32 font_height = 8;

	//Entities loading
	entity* player = entity_create();
	setup_player(player);

	//testing the new entity system
	float resource_range = 300;
	int resource_count = 30;
	for (int i = 0; i < resource_count; i++){
		entity* en = entity_create();
		setup_rock(en);
		en->pos = v2(get_random_float32_in_range(-resource_range, resource_range), get_random_float32_in_range(-resource_range, resource_range));
		en->pos = round_v2_to_tile(en->pos);
		en->pos.y -= tile_size * -0.5;
	}
	for (int i = 0; i < resource_count; i++){
		entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-resource_range, resource_range), get_random_float32_in_range(-resource_range, resource_range));
		en->pos = round_v2_to_tile(en->pos);
		en->pos.y -= tile_size * 0.5;
	}

	
	float64 lastTime = os_get_current_time_in_seconds();
	float64 secondsCounter = 0.0;
	s32 frameCount = 0.0;

	float zoom = 4.3;
	Vector2 camera_pos = v2(0,0);
	

	while (!window.should_close) {
		reset_temporary_storage();
		
		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - lastTime;
		lastTime = now;

		os_update();

		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		// for camera
		{
		Vector2 target = player->pos;
			animate_v2_to_target(&camera_pos, target, delta_t, 5.0f);
			//camera_pos = 
			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 1.0)));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0/zoom, 1.0/zoom, 1.0)));
		}

		Vector2 mouse_pos = screen_to_world();

		/*{ // mouse detection / collision example 

			for (int i = 0; i < MAX_ENTITIES; i++){ 
				entity* en = &world->entities[i]; //all entities
				if (en->is_exist){ //getting all entities that exist
					//setting up the collision
					sprite* spr = get_sprite(en->id);
					Range2f r = range2f_make_bottom_center(spr->size);
					r = range2f_shift(r, en->pos);

					Vector4 colour = COLOR_BLUE; //set debug color
					colour.a = 0.3;

					if(range2f_contains(r, mouse_pos)){ //if mouse is over square
						colour.a = 1.0;
					}

					//draw_rect(r.min, range2f_size(r), colour);
				}

			}
		

		} */

		{
			

			for (int i = 0; i < MAX_ENTITIES; i++){ 
				entity* en = &world->entities[i]; //all entities
				if (en->is_exist){ //getting all entities that exist
					//setting up the collision
					sprite* spr = get_sprite(en->id);					
					
					if(fabsf(v2_dist(en->pos, mouse_pos)) < entity_selection_radious){

						int x_pos = world_to_tile_pos(en->pos.x);
						int y_pos =  world_to_tile_pos(en->pos.y);

						draw_rect( //draw mouse tile
							v2(
								tile_pos_to_world_pos(x_pos) + (float)tile_size * -0.5, 
								tile_pos_to_world_pos(y_pos) + (float)tile_size * -0.5
							),
							v2(tile_size, tile_size), 
							COLOR_BLUE
						);
					}
				}
			}		
		}

		{ //for drawing the checkered background

			int mouse_tile_x = world_to_tile_pos(mouse_pos.x);
			int mouse_tile_y = world_to_tile_pos(mouse_pos.y);

			int pl_pos_x = world_to_tile_pos(player->pos.x);
			int pl_pos_y = world_to_tile_pos(player->pos.y);		
			int tile_rad_x = 25;
			int tile_rad_y = 16;
			Vector4 tile_color = COLOR_BROWN;


			for (int x = pl_pos_x - tile_rad_x; x < pl_pos_x + tile_rad_x; x++){
				for (int y = pl_pos_y - tile_rad_y; y <  pl_pos_y + tile_rad_y; y++){
					if((x + (y % 2 == 0) ) % 2 == 0){

						float x_pos = x * tile_size;	
						float y_pos =  y * tile_size;

						draw_rect(
							v2(
								x_pos + (float)tile_size * -0.5, 
								y_pos + (float)tile_size * -0.5
							),
							v2(tile_size, tile_size), 
							tile_color
						);
					}
				}
			}

			draw_rect( //draw mouse tile
				v2(
					tile_pos_to_world_pos(mouse_tile_x) + (float)tile_size * -0.5, 
					tile_pos_to_world_pos(mouse_tile_y) + (float)tile_size * -0.5
				),
				v2(tile_size, tile_size), 
				COLOR_BLUE
			);
		}

		//drawing entities
		for (int i = 0; i < MAX_ENTITIES; i++){
			entity* en = &world->entities[i];
			if(en->is_exist){

				switch (en->type) {

				default:
					
					sprite* s = get_sprite(en->id);

					Matrix4 xform = m4_scalar(1.0);
					xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
					xform = m4_translate(xform, v3(s->size.x * -0.5, 0.0, 0));

					draw_image_xform(s->image, xform, s->size, en->color);					

					/*
					draw_text(font, 
						tprint("Positon: %f, %f", en->pos.x, en->pos.y), 
						font_height, v2(en->pos.x - 10, en->pos.y - 10), v2(0.5,0.5), COLOR_BLUE);
					*/
				}
			}
		}

		if (is_key_just_pressed(KEY_ESCAPE)){ // for closing the window / game
			window.should_close = true;
		}

		Vector2 inputAxis = v2(0, 0);
		if (is_key_down('A')) {
			inputAxis.x -= 1.0;
		}
		if (is_key_down('D')) {
			inputAxis.x += 1.0;
		}
		if (is_key_down('S')) {
			inputAxis.y -= 1.0;
		}
		if (is_key_down('W')) {
			inputAxis.y += 1.0;
		}
		inputAxis = v2_normalize(inputAxis);
		
		//playerPosition = playerPosition + (inputAxis * 30.0);
		player->pos = v2_add(player->pos, v2_mulf(inputAxis, player->speed * delta_t));

		gfx_update();

		//log_info("fps: %i", secondsCounter); 

		secondsCounter += delta_t;
		frameCount += 1;
		if(secondsCounter >= 1.0){ 
			log("fps: %i", frameCount); 
			secondsCounter = 0;
			frameCount = 0;
		}
	}
	return 0;
}