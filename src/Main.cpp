#pragma once

#include "Prelude.h"
#include "Random.h"
#include <string>
#include <cmath>
#include <iostream>
#include<windows.h>

using namespace core;

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
};
struct MonsterHealthbar
{
    int value;
};

enum Option{
	HAND=1,
	SHIELD,
	GUN,
	SHORD,
	BOMB,
	KNIFE,
	NMP
};
struct Cursor {};

struct Monster{
	String name;
	int health;
};

struct Jane{
	int health;
	int monster_score=0;
	float attack_strength=1.0;
	float monster_coeff=1.0;
};
struct JaneHealthbar{
	int value;
};


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define CURSOR_STEP 65

#define MENUBG_PIVOT_X SCREEN_WIDTH/2
#define MENUBG_PIVOT_Y SCREEN_HEIGHT

#define MENUBG_WIDTH SCREEN_WIDTH
#define MENUBG_HEIGHT 245

#define MENUBG_X MENUBG_PIVOT_X-MENUBG_WIDTH/2
#define MENUBG_Y MENUBG_PIVOT_Y-MENUBG_HEIGHT

#define ATTACK_X 1
#define ATTACK_Y 1

struct OptionIndexSignal{
    int i;
};
struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Cursor, KeyBindings, Position>
    , public SignalEmitter<OptionIndexSignal>
{
	int idx=1;
    void on_tick() override
	{
		const auto& keys = KeyState::get(); 

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto&& [entity, bindings, pos] : access_storage().each())
		{
			
			if (keys.is_pressed(bindings.up)) 
			{
				if(pos.xy.y-CURSOR_STEP>=MENUBG_Y+50){
                    pos.xy.y -= CURSOR_STEP;
                    idx-=1;
                }
					 
			}
			else if (keys.is_pressed(bindings.down)) 
			{
				if(pos.xy.y+CURSOR_STEP<=MENUBG_PIVOT_Y-50){
                    pos.xy.y += CURSOR_STEP;
                    idx+=1;
                }	  
			}
			else if(keys.is_pressed(bindings.left)){
				if(pos.xy.x-320>0){
                    pos.xy.x-=320;
                    idx-=3;
                }
					
			}
			else if(keys.is_pressed(bindings.right)){
				if(pos.xy.x+320<SCREEN_WIDTH){
                    pos.xy.x+=320;
                    idx+=3;
                }		
			}
            else if(keys.is_pressed(KEY_SPACE)){
                    emit(OptionIndexSignal{idx});
            }
		}
	}
};
struct AttackSignal
	{
		int id;
	};
struct MonsterAttackSignal
{

};
struct BlockSignal{};

struct ChoosingOptionSystem
    : public ecs::System
	, public SignalProcessor<OptionIndexSignal>
    , public SignalEmitter<AttackSignal>
	, public SignalEmitter<MonsterAttackSignal>
	, public SignalEmitter<BlockSignal>
{
    virtual void process_signal(OptionIndexSignal& signal){
        switch (signal.i)
        {
        case 1:
            SignalEmitter<AttackSignal>::emit(AttackSignal{1});
            break;
        case 2:
            SignalEmitter<BlockSignal>::emit(BlockSignal{});
			break;
        case 3:
           SignalEmitter<MonsterAttackSignal>::emit(MonsterAttackSignal{});
		   break;
		case 4:
			SignalEmitter<AttackSignal>::emit(AttackSignal{2});
			break;
		case 5:
			break;
		case 6:
			SignalEmitter<AttackSignal>::emit(AttackSignal{3});
			break;
		}
		SignalEmitter<MonsterAttackSignal>::emit(MonsterAttackSignal{});
    }
};
struct AttackSystem
    : public ecs::System
    , public SignalProcessor<AttackSignal>
	, public SignalProcessor<MonsterAttackSignal>
	, public SignalEmitter<MonsterAttackSignal>
    , public MutAccessGroupStorage<MonsterHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<JaneHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<Monster>
	, public MutAccessGroupStorage<Jane>
{
    
	String health_sprites[10]; //scale za sprite

	AttackSystem()
	{
		String base_string="nexus/healthbar_";

		for(int i=0;i<10;i++)
		{
			health_sprites[i]=base_string+std::to_string(i);
		}
	}

	virtual void process_signal(AttackSignal& signal)
	{	
		for (auto &&[entity,monster] : MutAccessGroupStorage<Monster>::access_storage().each())
		{
			int harm=get_harm(signal.id);
			monster.health=std::max(0,monster.health-harm);
		}
    }
	virtual void process_signal(MonsterAttackSignal& signal)
	{
		for(auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each()){
			int harm=10;
			jane.health=std::max(0,jane.health-harm);
			jane.monster_score+=10;
		}
    }

	String get_sprite(int health)
	{
		return health_sprites[health/10];
	}
	int get_harm(int id)
	{
		//TODO: verovatnoca
		if(id==1)
		{
			return 4; 
		}
		else if(id==2)
		{
			return 8;
		}
		else
			return 6;
	}
};
struct MonsterMovement
	: public ecs::System
	, public MutAccessGroupStorage<Monster, Position>
{
	int num=0;
	float velocity=1;
	bool up=true;
	
	void on_tick() override
	{
		
		for (auto&& [entity,monster,pos] : access_storage().each())
		{
			if(up){
				num++;
				pos.xy.y+=velocity*Time::delta_time()*22.0f;
				if(num==350){
					up=false;
					num=0;
				}
			}
			else{
				num++;
				pos.xy.y-=velocity*Time::delta_time()*22.0f;
				if(num==350){
					up=true;
					num=0;
				}
			}
			
		}
	}
};
struct HealthbarSpriteAnimationSystem:
	public ecs::System
	, public MutAccessGroupStorage<MonsterHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<JaneHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<Monster>
	, public MutAccessGroupStorage<Jane>
{
	void on_tick() override 
	{
		for (auto &&[entity,monster] : MutAccessGroupStorage<Monster>::access_storage().each())
		{
			for (auto &&[healthbar_entity, healthbar,sprite] : MutAccessGroupStorage<MonsterHealthbar,SpriteAnimation>::access_storage().each())
			{
				sprite.change_to("nexus/healthbar_"+std::to_string(monster.health/10));
			}
		}

		for (auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
		{
			for (auto &&[healthbar_entity, healthbar,sprite] : MutAccessGroupStorage<JaneHealthbar,SpriteAnimation>::access_storage().each())
			{
				sprite.change_to("nexus/healthbar_"+std::to_string(jane.health/10));
			}
		}
	}
};
struct JaneSpriteAnimationSystem: 
	public ecs::System, 
	public MutAccessGroupStorage<Jane, SpriteAnimation>
{
	void on_tick() override
	{
		for (auto&& [entity,jane,sprite] : access_storage().each()){
			if(jane.monster_score>50)
			{
				sprite.change_to("nexus/jane_stage4");
			}
			else if(jane.monster_score>40)
			{
				sprite.change_to("nexus/jane_stage3");
			}
			else if(jane.monster_score>30)
			{
				sprite.change_to("nexus/jane_stage2");
			}
			else if(jane.monster_score>20)
			{
				sprite.change_to("nexus/jane_stage1");
			}
		}
	}
};
struct Pong : public Game
{
	Pong()
	{
		
		auto& engine = Engine::get_instance();
		engine.use<MonsterMovement>();
		engine.use<JaneSpriteAnimationSystem>();
		engine.use<HealthbarSpriteAnimationSystem>();
		engine.use<PlayerControlsSystem>();
        engine.use<ChoosingOptionSystem>();
		engine.use<AttackSystem>();
	}

	void on_start() override
	{
	
	 
        auto button_attack = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/sword"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55 })
			.with<Visibility>(true)
			.done();
        auto button_block = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/sword"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55+65})
			.with<Visibility>(true)
			.done(); 
        auto button_items = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/sword"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55+65+65})
			.with<Visibility>(true)
			.done();  
        auto sword = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/sword"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55})
			.with<Visibility>(true)
			.done();  
		auto shield = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/shield"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55 + 65})
			.with<Visibility>(true)
			.done();
		auto knife = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/knife"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55 + 65 + 65})
			.with<Visibility>(true)
			.done();       
		auto menubg = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/menubg"))
			.with<Position>(geometry::Vec2{ MENUBG_PIVOT_X, MENUBG_PIVOT_Y })
			.with<Visibility>(true)
			.done();

		auto menucursor = spawn()
			.with<Cursor>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/menucursor"))
			.with<Position>(geometry::Vec2{ MENUBG_X+50, MENUBG_Y+55 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S,KeyCode::KEY_A,KeyCode::KEY_D)
			.done();
		
		auto health = spawn()
            .with<MonsterHealthbar>(100)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/healthbar_10"))
			.with<Position>(geometry::Vec2{ SCREEN_WIDTH-300, 80 })
			.with<Visibility>(true)
			.done();
		health = spawn()
			.with<JaneHealthbar>(100)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/healthbar_10"))
			.with<Position>(geometry::Vec2{ 300, 80 })
			.with<Visibility>(true)
			.done();
		auto monster = spawn()
			.with<Monster>("Gluttony",100)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/monster"))
			.with<Position>(geometry::Vec2{SCREEN_WIDTH-200,SCREEN_HEIGHT-260})
			.with<Visibility>(true)
			.done();	
		auto jane = spawn()
			.with<Jane>(100 )
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/jane"))
			.with<Position>(geometry::Vec2{200,SCREEN_HEIGHT-260})
			.with<Visibility>(true)
			.done();
		// auto background=spawn()
		// 	.with<Sprite>(ecs::no_entity)
		// 	.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/background"))
		// 	.with<Position>(geometry::Vec2{ MENUBG_X+SCREEN_WIDTH/2, MENUBG_Y})
		// 	.with<Visibility>(true)
		// 	.done();
		
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	Pong game;
	engine.run();

	return 0;
}
