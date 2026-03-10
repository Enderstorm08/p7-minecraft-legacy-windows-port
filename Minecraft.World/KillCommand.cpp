#include "stdafx.h"
#include "net.minecraft.commands.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.damagesource.h"
#include "KillCommand.h"

EGameCommand KillCommand::getId()
{
	return eGameCommand_Kill;
}

void KillCommand::execute(shared_ptr<CommandSender> source, byteArray commandData)
{
	shared_ptr<Player> player = dynamic_pointer_cast<Player>(source);
	if (player == NULL || player->removed || player->getHealth() <= 0)
	{
		return;
	}

	player->hurt(DamageSource::outOfWorld, player->getHealth() + player->getMaxHealth());
}
