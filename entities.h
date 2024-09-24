typedef struct sprite{
	Gfx_Image* image;
	Vector2 size; 
}sprite;

typedef enum spriteID{
	SPRITE_NIL,
	SPRITE_PLAYER,
	SPRITE_TREE0,
	SPRITE_TREE1,
	SPRITE_ROCK0,
	SPRITE_ROCK1,
	SPRITE_LOG_DROP,
	SPRITE_MAX,

}spriteID;

sprite sprites[SPRITE_MAX];

sprite* get_sprite(spriteID id){
	if (id >= 0 && id <= SPRITE_MAX){
		return &sprites[id];
	}
	return &sprites[0];
}

typedef enum entity_type {
	TYPE_NIL = 0,
	TYPE_PLAYER = 1,
	TYPE_TREE = 2,
	TYPE_LOG_DROP = 3,
	TYPE_ROCK = 4,
	
} entity_type;

typedef struct entity {
	Vector2 pos;
	Vector4 color;
	float speed;
	entity_type type;
	bool is_exist;
	bool render;

	spriteID id;
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
	en->type = TYPE_ROCK;
	en->color = COLOR_WHITE;
	en->is_exist = true;
	en->render = true;
	en->id = SPRITE_ROCK0;
}
void setup_tree(entity* en){
	en->type = TYPE_TREE;
	en->color = hex_to_rgba(0x7cb06dff);
	en->is_exist = true;
	en->render = true;
	en->id = SPRITE_TREE0;
	//en->id = SPRITE_TREE1;
}

void setup_player(entity* en){
	en->pos = v2(1,1);
	en->speed = 150;
	en->color = COLOR_WHITE;
	en->type = TYPE_PLAYER;
	en->id = SPRITE_PLAYER;
	en->is_exist = true;
	en->render = true;
}