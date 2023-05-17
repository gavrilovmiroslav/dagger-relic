#pragma once

#include "Prelude.h"
#include "Random.h"
#include <string>
#include <cmath>
#include <iostream>

#include <vector>
#include <random>
#include <algorithm>
#include<windows.h>

#include <chrono> // za rad sa vremenom
#include <thread>

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
	SWORD,
	KNIFE,
	BOMB,
	FOOD,
	POTION,
	SHOTGUN
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
	bool shield=false;
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
struct Fire
{

};
struct MonsterAttackSignal
{

};
struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Cursor, KeyBindings, Position>
	, public MutAccessGroupStorage<Fire>
    , public SignalEmitter<OptionIndexSignal>
	, public SignalEmitter<MonsterAttackSignal>
{
	int idx=1;
	int fire_i=0;
	int monster_i=0;
	bool calc=false;

    void on_tick() override
	{
		const auto& keys = KeyState::get(); 
		fire_i++;
		monster_i++;

		if (keys.is_pressed(KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

		for (auto&& [entity, bindings, pos] : MutAccessGroupStorage<Cursor, KeyBindings, Position>::access_storage().each())
		{
			
			if (keys.is_pressed(bindings.up)) 
			{
				if(pos.xy.y-CURSOR_STEP>=MENUBG_Y+50)
				{
                    pos.xy.y -= CURSOR_STEP;
                    idx-=1;
                }
					 
			}
			else if (keys.is_pressed(bindings.down)) 
			{
				if(pos.xy.y+CURSOR_STEP<=MENUBG_PIVOT_Y-50)
				{
                    pos.xy.y += CURSOR_STEP;
                    idx+=1;
                }	  
			}
			else if(keys.is_pressed(bindings.left))
			{
				if(pos.xy.x-320>0){
                    pos.xy.x-=320;
                    idx-=3;
                }
					
			}
			else if(keys.is_pressed(bindings.right))
			{
				if(pos.xy.x+320<SCREEN_WIDTH*3/4)
				{
                    pos.xy.x+=320;
                    idx+=3;
                }		
			}
            else if(keys.is_pressed(KEY_SPACE) && !calc)
			{
                    SignalEmitter<OptionIndexSignal>::emit(OptionIndexSignal{idx});
					auto s=spawn()
						.with<Sprite>(ecs::no_entity)
						.with<Fire>()
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/fire"))
						.with<Visibility>(false)
						.with<Position>(geometry::Vec2{SCREEN_WIDTH-450,SCREEN_HEIGHT-200})
						.done();
					fire_i=0;
					calc=true;
            }
			else{
				for (auto&& [entity] : MutAccessGroupStorage<Fire>::access_storage().each())
				{
					if(fire_i==60*30 && calc==true)
					{
						despawn(entity);
						calc=false;
						SignalEmitter<MonsterAttackSignal>::emit(MonsterAttackSignal{});
						
					}
				}

			}
		
		}
	}
};
struct AttackSignal
{
		Option option;
};

struct BlockSignal{};

struct ChoosingOptionSystem
    : public ecs::System
	, public SignalProcessor<OptionIndexSignal>
    , public SignalEmitter<AttackSignal>
	, public SignalEmitter<MonsterAttackSignal>
	, public SignalEmitter<BlockSignal>
{
    
	virtual void process_signal(OptionIndexSignal& signal)
	{
        if(signal.i==Option::SHIELD)
		{
			SignalEmitter<BlockSignal>::emit(BlockSignal{});
		}
		else
		{
			SignalEmitter<AttackSignal>::emit(AttackSignal{static_cast<Option>(signal.i)});
		}
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

	virtual void process_signal(AttackSignal& signal)
	{	
		for (auto &&[entity,monster] : MutAccessGroupStorage<Monster>::access_storage().each())
		{
			for(auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
			{
				int harm=get_harm(signal.option);

				monster.health=std::max(0,monster.health-harm);
			}
		}
    }
	virtual void process_signal(MonsterAttackSignal& signal)
	{
		for(auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
		{
			for (auto &&[entity,monster] : MutAccessGroupStorage<Monster>::access_storage().each())
			{
				int harm=number_from_distribution({0.0,0.01,0.02,0.03,0.05,0.1,0.4,0.2,0.1,0.05,0.04},0,10);
				if(!jane.shield)
				{
					jane.health=std::max(0,jane.health-harm);
				}
				else
				{
					jane.shield=false;
					monster.health-=harm/2;
					jane.health-=number_from_distribution({0.4,0.3,0.2,0.1},0,3);
				}
			}
		}
    }

	int get_harm(Option option)
	{
		int harm=0;
		for(auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
		{
			switch (option)
			{
				case HAND:
					harm = number_from_distribution({0.0,0.05,0.1,0.15,0.3,0.4},0,5);
					jane.attack_strength+=0.05;
					break;
				case KNIFE:
					harm = number_from_distribution({0.0,0.01,0.02,0.05,0.08,0.15,0.65},0,6);
					jane.monster_score+=4;
					break;
				case BOMB:
					harm = number_from_distribution({0.0,0.0,0.0,0.0,0.0,0.01,0.02,0.03,0.04,0.1,0.8},0,10);
					jane.monster_score+=10;
					break;
				case GUN:
					harm = number_from_distribution({0.0,0.01,0.02,0.05,0.07,0.2,0.35,0.3},0,7);
					jane.monster_score+=7;
					break;
				case SWORD:
					harm = number_from_distribution({0.0,0.01,0.02,0.03,0.04,0.15,0.3,0.25,0.2},0,8);
					jane.monster_score+=5;
					break;
				case SHIELD:
					harm=0;
					jane.shield=true;
					jane.monster_score-=5;
					break;
				case FOOD:
					harm=0;
					jane.health=std::min(100,jane.health+20);
					jane.monster_score-=10;
					jane.attack_strength+=0.2;
					break;
				case POTION:
					harm=0;
					jane.monster_score-=20;
					jane.attack_strength+=0.3;
					break;
				case SHOTGUN:
					harm = number_from_distribution({0.0,0.0,0.01,0.02,0.03,0.04,0.1,0.3,0.5},0,8);
					jane.monster_score+=8;
					break;
				default:
					harm=0;
					break;
			}
			return (int)std::round(jane.attack_strength*harm);
		}
		
	}

	int number_from_distribution(const std::vector<double>& distribution_vector, int begin, int end)
	{
		std::random_device rd;
		std::mt19937 generator(rd());

		std::discrete_distribution<> distribution(distribution_vector.begin(), distribution_vector.end());

		return begin + distribution(generator);
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
			if(up)
			{
				num++;
				pos.xy.y+=velocity*Time::delta_time()*22.0f;
				if(num==350)
				{
					up=false;
					num=0;
				}
			}
			else
			{
				num++;
				pos.xy.y-=velocity*Time::delta_time()*22.0f;
				if(num==350)
				{
					up=true;
					num=0;
				}
			}
			
		}
	}
};
struct EndingSignal{
	Monster monster;
	Jane jane;
};

struct HealthbarSpriteAnimationSystem:
	public ecs::System
	, public MutAccessGroupStorage<MonsterHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<JaneHealthbar,SpriteAnimation>
	, public MutAccessGroupStorage<Monster>
	, public MutAccessGroupStorage<Jane>
	, public SignalEmitter<EndingSignal>
{
	
	void on_tick() override 
	{
		EndingSignal s{};
		for (auto &&[entity,monster] : MutAccessGroupStorage<Monster>::access_storage().each())
		{
			for (auto &&[healthbar_entity, healthbar,sprite] : MutAccessGroupStorage<MonsterHealthbar,SpriteAnimation>::access_storage().each())
			{
				sprite.change_to("nexus/healthbar_"+std::to_string(monster.health/10));
			}
			s.monster=monster;
		}

		for (auto &&[entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
		{
			for (auto &&[healthbar_entity, healthbar,sprite] : MutAccessGroupStorage<JaneHealthbar,SpriteAnimation>::access_storage().each())
			{
				sprite.change_to("nexus/healthbar_"+std::to_string(jane.health/10));
			}
			s.jane=jane;
		}
		emit(s);
	}
};
struct EndingSystem
	: public ecs::System
	, public SignalProcessor<EndingSignal>
{
	virtual void process_signal(EndingSignal& signal) 
	{
		
				if(signal.jane.health<10 && signal.jane.monster_score<35)
				{
					spawn()
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/less_bad_end"))
						.with<Position>(geometry::Vec2{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2})
						.with<Visibility>(true)
						.done();
					
				}
				else if(signal.jane.health<10 && signal.jane.monster_score>=35)
				{
					spawn()
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/bad_end"))
						.with<Position>(geometry::Vec2{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2})
						.with<Visibility>(true)
						.done();
					
				}
				else if(signal.monster.health<10 && signal.jane.monster_score<35)
				{
					spawn()
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/good_end"))
						.with<Position>(geometry::Vec2{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2})
						.with<Visibility>(true)
						.done();
						
				}
				else if(signal.monster.health<10 && signal.jane.monster_score>=35)
				{
					spawn()
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/less_good_end"))
						.with<Position>(geometry::Vec2{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2})
						.with<Visibility>(true)
						.done();
					
				}
	}
};
struct JaneSpriteAnimationSystem: 
	public ecs::System, 
	public MutAccessGroupStorage<Jane, SpriteAnimation>
{
	void on_tick() override
	{
		for (auto&& [entity,jane,sprite] : access_storage().each())
		{
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
struct Comment {};
struct MonsterCommentsSystem: 
	public ecs::System, 
	public MutAccessGroupStorage<Jane>,
	public MutAccessGroupStorage<Comment>
	
{
	int time=0;
	bool show_weak_comment=false;
	
	void on_tick() override
	{
		time++;
		for (auto&& [entity,jane] : MutAccessGroupStorage<Jane>::access_storage().each())
		{
			if(jane.health<=90){
				spawn()
						.with<Comment>()
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/weak"))
						.with<Position>(geometry::Vec2{ SCREEN_WIDTH-200, SCREEN_HEIGHT-400})
						.with<Visibility>(true)
						.done();
				time=0;
				show_weak_comment=true;
			}
		}
		if(time==60*10 && show_weak_comment==true)
		{
			for(auto&& [entity] : MutAccessGroupStorage<Comment>::access_storage().each())
			{
				despawn(entity);
			}
			show_weak_comment=false;
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
		engine.use<EndingSystem>();
		//engine.use<MonsterCommentsSystem>();
	}

	void on_start() override
	{
		
        auto hand = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/hand"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55 })
			.with<Visibility>(true)
			.done();
        auto shield = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/shield"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55+65})
			.with<Visibility>(true)
			.done(); 
        auto gun = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/gun"))
			.with<Position>(geometry::Vec2{ MENUBG_X+175, MENUBG_Y+55+65+65})
			.with<Visibility>(true)
			.done();  
        auto sword = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/sword"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55})
			.with<Visibility>(true)
			.done();  
		auto knife = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/knife"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55 + 65})
			.with<Visibility>(true)
			.done();
		auto bomb = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/bomb"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450, MENUBG_Y+55 + 65 + 65})
			.with<Visibility>(true)
			.done();
		auto food = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/food"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450+300, MENUBG_Y+55})
			.with<Visibility>(true)
			.done();
		auto potion = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/potion"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450+300, MENUBG_Y+55+65})
			.with<Visibility>(true)
			.done();
		auto shotgun = spawn()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("nexus/shotgun"))
			.with<Position>(geometry::Vec2{ MENUBG_X+450+300, MENUBG_Y+55+65+70})
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
			.with<Position>(geometry::Vec2{SCREEN_WIDTH-250,SCREEN_HEIGHT-260})
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
