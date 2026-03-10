#include "stdafx.h"
#include "TheEndPortal.h"
#include "TheEndPortalTileEntity.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.dimension.h"
#include "net.minecraft.world.level.storage.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.h"

DWORD TheEndPortal::tlsIdx = TlsAlloc();

namespace
{
	void BuildGatewayLanding(Level *level, int x, int y, int z)
	{
		for (int xx = x - 2; xx <= x + 2; ++xx)
		{
			for (int zz = z - 2; zz <= z + 2; ++zz)
			{
				for (int yy = y - 3; yy <= y - 1; ++yy)
				{
					if (level->isEmptyTile(xx, yy, zz))
					{
						level->setTile(xx, yy, zz, Tile::whiteStone_Id);
					}
				}
			}
		}

		for (int xx = x - 1; xx <= x + 1; ++xx)
		{
			for (int zz = z - 1; zz <= z + 1; ++zz)
			{
				level->setTile(xx, y - 1, zz, Tile::unbreakable_Id);
			}
		}

		for (int yy = y; yy <= y + 3; ++yy)
		{
			level->setTile(x, yy, z, 0);
		}
	}
}

// 4J - allowAnywhere is a static in java, implementing as TLS here to make thread safe
bool TheEndPortal::allowAnywhere()
{
	return (TlsGetValue(tlsIdx) != NULL);
}

void TheEndPortal::allowAnywhere(bool set)
{
	TlsSetValue(tlsIdx,(LPVOID)(set?1:0));
}

TheEndPortal::TheEndPortal(int id, Material *material) : EntityTile(id, material, isSolidRender())
{
    this->setLightEmission(1.0f);
}

shared_ptr<TileEntity> TheEndPortal::newTileEntity(Level *level)
{
	return shared_ptr<TileEntity>(new TheEndPortalTileEntity());
}

void TheEndPortal::updateShape(LevelSource *level, int x, int y, int z, int forceData, shared_ptr<TileEntity> forceEntity) // 4J added forceData, forceEntity param
{
    float r = 1 / 16.0f;
    this->setShape(0, 0, 0, 1, r, 1);
}

bool TheEndPortal::shouldRenderFace(LevelSource *level, int x, int y, int z, int face)
{
    if (face != 0) return false;
    return EntityTile::shouldRenderFace(level, x, y, z, face);
}

void TheEndPortal::addAABBs(Level *level, int x, int y, int z, AABB *box, AABBList *boxes, shared_ptr<Entity> source)
{
}

bool TheEndPortal::isSolidRender(bool isServerLevel)
{
	return false;
}

bool TheEndPortal::isCubeShaped()
{
	return false;
}

int TheEndPortal::getResourceCount(Random *random)
{
	return 0;
}

void TheEndPortal::entityInside(Level *level, int x, int y, int z, shared_ptr<Entity> entity)
{
    if (entity->riding == NULL && entity->rider.lock() == NULL)
	{
        if (dynamic_pointer_cast<Player>(entity) != NULL)
		{
            if (!level->isClientSide)
			{
				// 4J Stu - Update the level data position so that the stronghold portal can be shown on the maps
				int x,z;
				x = z = 0;
				if(level->dimension == 0 && !level->getLevelData()->getHasStrongholdEndPortal() && app.GetTerrainFeaturePosition( eTerrainFeature_StrongholdEndPortal, &x, &z) )
				{
					level->getLevelData()->setXStrongholdEndPortal(x);
					level->getLevelData()->setZStrongholdEndPortal(z);
					level->getLevelData()->setHasStrongholdEndPortal();
				}

                (dynamic_pointer_cast<Player>(entity))->changeDimension(1);
            }
        }
    }
}

void TheEndPortal::animateTick(Level *level, int xt, int yt, int zt, Random *random)
{
    double x = xt + random->nextFloat();
    double y = yt + 0.8f;
    double z = zt + random->nextFloat();
    double xa = 0;
    double ya = 0;
    double za = 0;

    level->addParticle(eParticleType_endportal, x, y, z, xa, ya, za);
}

int TheEndPortal::getRenderShape()
{
    return SHAPE_INVISIBLE;
}

void TheEndPortal::onPlace(Level *level, int x, int y, int z)
{
    if (allowAnywhere()) return;

    if (level->dimension->id != 0)
	{
        level->setTile(x, y, z, 0);
        return;
    }
}

int TheEndPortal::cloneTileId(Level *level, int x, int y, int z)
{
	return 0;
}

void TheEndPortal::registerIcons(IconRegister *iconRegister)
{
	// don't register null, because of particles
	icon = iconRegister->registerIcon(L"portal");
}

EndGatewayTile::EndGatewayTile(int id, Material *material) : TheEndPortal(id, material)
{
}

void EndGatewayTile::entityInside(Level *level, int x, int y, int z, shared_ptr<Entity> entity)
{
	if (level->isClientSide || level->dimension->id != LevelData::DIMENSION_END)
	{
		return;
	}

	if (entity->riding != NULL || entity->rider.lock() != NULL)
	{
		return;
	}

	shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(entity);
	if (mob == NULL)
	{
		return;
	}

	LevelData *levelData = level->getLevelData();
	if (levelData == NULL || !levelData->getHasEndGateway())
	{
		return;
	}

	int sourceX = levelData->getXEndGateway();
	int sourceY = levelData->getYEndGateway();
	int sourceZ = levelData->getZEndGateway();
	int targetX = levelData->getXOuterEndGateway();
	int targetY = levelData->getYOuterEndGateway();
	int targetZ = levelData->getZOuterEndGateway();

	if (abs(x - sourceX) > 2 || abs(z - sourceZ) > 2)
	{
		targetX = sourceX;
		targetY = sourceY;
		targetZ = sourceZ;
	}

	if (dynamic_pointer_cast<Player>(mob) != NULL)
	{
		shared_ptr<Player> player = dynamic_pointer_cast<Player>(mob);
		if (player->changingDimensionDelay > 0)
		{
			return;
		}
		player->changingDimensionDelay = 20;
	}

	if (targetY <= 0)
	{
		targetY = max(level->seaLevel + 4, level->getTopSolidBlock(targetX, targetZ));
	}

	BuildGatewayLanding(level, targetX, targetY, targetZ);
	level->setTile(targetX, targetY, targetZ, Tile::endGateway_Id);
	mob->teleportTo(targetX + 0.5f, targetY + 1.0f, targetZ + 0.5f);
}

void EndGatewayTile::onPlace(Level *level, int x, int y, int z)
{
	if (allowAnywhere())
	{
		return;
	}

	if (level->dimension->id != LevelData::DIMENSION_END)
	{
		level->setTile(x, y, z, 0);
	}
}
