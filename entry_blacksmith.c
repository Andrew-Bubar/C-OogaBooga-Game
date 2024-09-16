
typedef enum entity_type {
	type_nil = 0,
	type_player = 1,
	type_tree = 2,
	type_log_drop = 3,
	type_rock = 4,
	

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
	en->type = type_rock;
	en->sprite = load_image_from_disk(fixed_string("rock0.png"), get_heap_allocator());
	en->scale = v2(25, 25);
	en->color = COLOR_WHITE;
	en->is_exist = true;
	en->render = true;
}
void setup_tree(entity* en){
	en->type = type_tree;
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
	en->type = type_player;
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
		en->pos = v2(get_random_float32_in_range(-500, 500), get_random_float32_in_range(-500, 500));
	}
	for (int i = 0; i < 30; i++){
		entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-500, 500), get_random_float32_in_range(-500, 500));
	}
	
	float64 lastTime = os_get_current_time_in_seconds();
	float64 secondsCounter = 0.0;
	s32 frameCount = 0.0;

	while (!window.should_close) {
		reset_temporary_storage();
		
		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - lastTime;
		lastTime = now;

		os_update();

		//drawing entities
		for (int i = 0; i < MAX_ENTITIES; i++){
			entity* en = &world->entities[i];
			if(en->is_exist){

				switch (en->type)
				{

				
				case (type_player):{
					
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