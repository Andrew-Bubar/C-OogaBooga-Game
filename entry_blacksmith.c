
typedef enum entity_type {
	TYPE_NIL = 0,
	TYPE_PLAYER = 1,
	TYPE_TREE = 2,
	TYPE_LOG_DROP = 3,
	TYPE_ROCK = 4,
	

} entity_type;

typedef struct entity {
	Vector2 pos;
	Vector2 scale;
	Gfx_Image* sprite;
	Vector4 color;
	float speed;
	entity_type type;
	bool is_exist;
	bool render;
} entity;

typedef struct sprite{
	Gfx_Image* image;
}sprite;

typedef enum spriteID{
	SPRITE_NIL = 0,
	SPRITE_PLAYER = 1,
	SPRITE_TREE0 = 2,
	SPRITE_TREE1 = 3,
	SPRITE_ROCK0 = 4,
	SPRITE_ROCK1 = 5,
	SPRITE_LOG_DROP = 6,
	SPRITE_MAX,

}spriteID;

sprite sprites[SPRITE_MAX];

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

#define MAX_ENTITIES 1024
typedef struct World{
	entity entities[MAX_ENTITIES];
} World;

World* world = 0;

entity* entity_create(){
	entity* found = 0;
	for(int i = 0; i < MAX_ENTITIES; i++){
		entity* exists = &world->entities[i];
		if(!exists->is_exist){
			found = exists;
			break;
		}
	}
	assert(found, "No more memory for entities");
	return found;
}

void entity_destory(entity* ent){
	memset(ent, 0, sizeof(entity)); 
}

//making the different entities
void setup_rock(entity* en){
	en->type = TYPE_ROCK;
	en->sprite = load_image_from_disk(fixed_string("rock0.png"), get_heap_allocator());
	en->scale = v2(25, 25);
	en->color = COLOR_WHITE;
	en->is_exist = true;
	en->render = true;
}
void setup_tree(entity* en){
	en->type = TYPE_TREE;
	en->sprite = load_image_from_disk(fixed_string("tree0.png"), get_heap_allocator());
	en->scale = v2(60, 60);
	en->color = COLOR_BROWN;
	en->is_exist = true;
	en->render = true;
}

void setup_player(entity* en){
	en->pos = v2(0,0);
	en->scale = v2(30, 30);
	en->speed = 300;
	en->sprite = load_image_from_disk(fixed_string("player.png"), get_heap_allocator());
	assert(en->sprite, "broke :(");
	en->color = COLOR_WHITE;
	en->type = TYPE_PLAYER;
	en->is_exist = true;
	en->render = true;
}

int entry(int argc, char **argv) {
	
	// This is how we (optionally) configure the window.
	// To see all the settable window properties, ctrl+f "struct Os_Window" in os_interface.c
	window.title = STR("Blacksmith Adventures");
	//0x306082ff (demo bg)
	window.clear_color = hex_to_rgba(0x836953ff);
	window.width = 1280;
	window.height = 720;

	//world start
	world = alloc(get_heap_allocator(), sizeof(World));

	//Entities loading
	entity* player = entity_create();
	setup_player(player);

	//testing the new entity system
	for (int i = 0; i < 30; i++){
		entity* en = entity_create();
		setup_rock(en);
		en->pos = v2(get_random_float32_in_range(-700, 500), get_random_float32_in_range(-700, 500));
	}
	for (int i = 0; i < 30; i++){
		entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-700, 500), get_random_float32_in_range(-1000, 1000));
	}
	
	float64 lastTime = os_get_current_time_in_seconds();
	float64 secondsCounter = 0.0;
	s32 frameCount = 0.0;

	float zoom = 2.3;
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
			animate_v2_to_target(&camera_pos, target, delta_t, 10.0f);
			//camera_pos = 
			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 1.0)));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0/zoom, 1.0/zoom, 1.0)));
		}

		Vector2 mouse_pos = screen_to_world();

		//drawing entities
		for (int i = 0; i < MAX_ENTITIES; i++){
			entity* en = &world->entities[i];
			if(en->is_exist){

				switch (en->type) {

				case (TYPE_PLAYER):{
					
				}

				case(TYPE_TREE):{
					if(mouse_pos.x == en->pos.x) { log("%i", en->type);}
				}

				default:
					draw_image(en->sprite, en->pos, en->scale, en->color);
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