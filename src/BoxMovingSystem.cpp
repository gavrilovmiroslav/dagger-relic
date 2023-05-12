#include "BoxMovingSystem.h"


//using namespace std;
void BoxMovingSystem::on_tick() {
    for (auto&& [entity, box, pos, gravity] : access_storage().each())
		{
			
            if(!box.isGrounded)
			    box.speed+=gravity.acceleration;
			
			pos.xy += box.speed * SPEED_MOD * Time::delta_time();

			if(pos.xy.y>570)
			{
				pos.xy.y=570;
				box.isGrounded=true;
				box.speed.y=0.0f;
			}

			if(pos.xy.x <= 15.0f)
				pos.xy.x = 15.0f;

			if(pos.xy.x >= 785.0f)
				pos.xy.x = 785.0f;	

			//cout<<"Visina roga "<<pos.xy.y<<endl;
		}
}