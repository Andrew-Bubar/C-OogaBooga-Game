

typedef struct entity {
	Vector2 pos;
	Vector2 scale;
	Gfx_Image* sprite;
	Vector4 color;
	float speed;
} entity;


int entry(int argc, char **argv) {
	
	// This is how we (optionally) configure the window.
	// To see all the settable window properties, ctrl+f "struct Os_Window" in os_interface.c
	window.title = STR("Blacksmith Adventures");
	//0x306082ff (demo bg)
	window.clear_color = hex_to_rgba(0x836953ff);
	window.width = 1280;
	window.height = 720;

	//Entities loading
	entity player;
	player.pos = v2(0,0);
	player.scale = v2(30, 30);
	player.speed = 300;
	player.sprite = load_image_from_disk(fixed_string("player.png"), get_heap_allocator());
	assert(player.sprite, "broke :(");
	player.color = COLOR_WHITE;

	entity tree0;
	tree0.pos = v2(-500, 200);
	tree0.scale = v2(60, 60);
	tree0.speed = 0;
	tree0.sprite = load_image_from_disk(fixed_string("tree1.png"), get_heap_allocator());
	assert(tree0.sprite, "broke :(");
	tree0.color = COLOR_WHITE;

	entity tree1;
	tree1.pos = v2(500, -300);
	tree1.scale = v2(60, 60);
	tree1.speed = 0;
	tree1.sprite = load_image_from_disk(fixed_string("tree2.png"), get_heap_allocator());
	assert(tree1.sprite, "broke :(");
	tree1.color = COLOR_WHITE;

	entity log;
	log.pos = v2(50, 0);
	log.scale = v2(25, 25);
	log.speed = 0;
	log.sprite = load_image_from_disk(fixed_string("log.png"), get_heap_allocator());
	assert(log.sprite, "broke :(");
	log.color = COLOR_BROWN;

	Gfx_Image* rock0 = load_image_from_disk(fixed_string("rock0.png"), get_heap_allocator());
	assert(rock0, "broke :(");
	Gfx_Image* rock1 = load_image_from_disk(fixed_string("rock1.png"), get_heap_allocator());
	assert(rock1, "broke :(");
	

	
	float64 lastTime = os_get_current_time_in_seconds();
	float64 secondsCounter = 0.0;
	s32 frameCount = 0.0;

	while (!window.should_close) {
		reset_temporary_storage();
		
		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - lastTime;
		lastTime = now;

		os_update();

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
		player.pos = v2_add(player.pos, v2_mulf(inputAxis, player.speed * delta_t));

		//rendering images
		draw_image(tree0.sprite, tree0.pos, tree0.scale, tree0.color);
		draw_image(tree1.sprite, tree1.pos, tree1.scale, tree1.color);
		draw_image(log.sprite, log.pos, log.scale, log.color);

		draw_image(rock0, v2(300, -200), v2(25, 25), COLOR_WHITE); //will figure out system before cave man the rest
		draw_image(rock1, v2(400, 250), v2(25, 25), COLOR_WHITE);

		draw_image(player.sprite, player.pos, player.scale, player.color); 

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