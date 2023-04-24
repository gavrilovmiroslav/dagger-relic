#include "Item.h"

void ItemSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	for (auto &&[player_entity, player, player_pos] : QueryPlayer::access_storage().each())
	{
		std::vector<entt::entity> remove_later;
		for (auto &&[item_entity, item, bindings, item_pos, sprite, item_visibility] : QueryItem::access_storage().each())
		{
			sprite.depth = -30;

			if (keys.is_down(bindings.up))
			{
				item.verticalVelocity = 200;
			}
			if (!ourGlobal.isGrounded)
			{
				item.verticalVelocity += GRAVITY * Time::delta_time();
				item_pos.xy.y -= item.verticalVelocity * Time::delta_time();
			}
			if (abs(player_pos.xy.x - item_pos.xy.x) < item.pickupRadius &&
			    abs(player_pos.xy.y - item_pos.xy.y) < item.pickupRadius)
			{
				player.coinNumber += 1;
				remove_later.push_back(item_entity);
				item_visibility.state = false;
			}
		}
		for (auto e : remove_later)
		{
			despawn(e);
		}
	}
}
