#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.biome.h"
#include "net.minecraft.world.level.levelgen.h"
#include "net.minecraft.world.level.levelgen.feature.h"
#include "net.minecraft.world.level.levelgen.structure.h"
#include "net.minecraft.world.level.levelgen.synth.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.tile.entity.h"
#include "net.minecraft.world.level.storage.h"
#include "ClothTile.h"
#include "QuartzBlockTile.h"
#include "TheEndLevelRandomLevelSource.h"

namespace
{
int FloorDivInt(int value, int divisor)
{
	if (value >= 0)
	{
		return value / divisor;
	}
	return -(((-value) + divisor - 1) / divisor);
}

bool IsEndStructureChunk(Level *level, int xt, int zt, int spacing, int separation, int salt)
{
	int regionX = FloorDivInt(xt, spacing);
	int regionZ = FloorDivInt(zt, spacing);

	Random featureRandom;
	featureRandom.setSeed((((__int64)regionX * 341873128712LL) ^ ((__int64)regionZ * 132897987541LL) ^ level->getSeed()) + salt);

	int candidateX = regionX * spacing + separation + featureRandom.nextInt(spacing - separation * 2);
	int candidateZ = regionZ * spacing + separation + featureRandom.nextInt(spacing - separation * 2);
	return xt == candidateX && zt == candidateZ;
}

int FindEndSurfaceY(Level *level, int x, int z)
{
	int y = level->getHeightmap(x, z);
	if (y >= Level::maxBuildHeight)
	{
		y = Level::maxBuildHeight - 1;
	}

	while (y > 1 && level->isEmptyTile(x, y, z))
	{
		--y;
	}

	return y;
}

bool HasEndFoundation(Level *level, int x, int z)
{
	int hits = 0;
	for (int dz = -2; dz <= 2; ++dz)
	{
		for (int dx = -2; dx <= 2; ++dx)
		{
			int groundY = FindEndSurfaceY(level, x + dx, z + dz);
			if (groundY < 40)
			{
				return false;
			}

			int tile = level->getTile(x + dx, groundY, z + dz);
			if (tile == Tile::whiteStone_Id || tile == Tile::quartzBlock_Id)
			{
				++hits;
			}
		}
	}

	return hits >= 20;
}

void PlaceBlock(Level *level, int x, int y, int z, int tile, int data = 0)
{
	level->setTileAndDataNoUpdate(x, y, z, tile, data);
}

void FillBox(Level *level, int x0, int y0, int z0, int x1, int y1, int z1, int tile, int data = 0)
{
	for (int y = y0; y <= y1; ++y)
	{
		for (int z = z0; z <= z1; ++z)
		{
			for (int x = x0; x <= x1; ++x)
			{
				PlaceBlock(level, x, y, z, tile, data);
			}
		}
	}
}

void HollowBox(Level *level, int x0, int y0, int z0, int x1, int y1, int z1, int tile, int data = 0)
{
	for (int y = y0; y <= y1; ++y)
	{
		for (int z = z0; z <= z1; ++z)
		{
			for (int x = x0; x <= x1; ++x)
			{
				bool edge = (x == x0 || x == x1 || y == y0 || y == y1 || z == z0 || z == z1);
				PlaceBlock(level, x, y, z, edge ? tile : 0, edge ? data : 0);
			}
		}
	}
}

void BuildSupportColumn(Level *level, int x, int startY, int z, int tile)
{
	for (int y = startY; y > 1; --y)
	{
		if (!level->isEmptyTile(x, y, z) && level->getTile(x, y, z) != Tile::whiteStone_Id)
		{
			break;
		}
		PlaceBlock(level, x, y, z, tile);
		if (!level->isEmptyTile(x, y - 1, z))
		{
			break;
		}
	}
}

void PlaceEndShipLoot(Level *level, int x, int y, int z)
{
	level->setTile(x, y, z, Tile::chest_Id);
	shared_ptr<ChestTileEntity> chest = dynamic_pointer_cast<ChestTileEntity>(level->getTileEntity(x, y, z));
	if (chest == NULL)
	{
		return;
	}

	chest->setItem(0, shared_ptr<ItemInstance>(new ItemInstance(Item::diamond, 5)));
	chest->setItem(4, shared_ptr<ItemInstance>(new ItemInstance(Item::enderPearl, 8)));
	chest->setItem(8, shared_ptr<ItemInstance>(new ItemInstance(Item::chestplate_diamond, 1)));
	chest->setItem(12, shared_ptr<ItemInstance>(new ItemInstance(Item::bow, 1)));
	chest->setItem(13, shared_ptr<ItemInstance>(new ItemInstance(Item::arrow, 16)));
	chest->setItem(17, shared_ptr<ItemInstance>(new ItemInstance(Item::netherQuartz, 12)));
}

void GenerateEndShip(Level *level, int x, int y, int z)
{
	FillBox(level, x, y, z - 2, x + 6, y, z + 2, Tile::quartzBlock_Id);
	FillBox(level, x + 1, y + 1, z - 2, x + 5, y + 2, z + 2, Tile::quartzBlock_Id);
	FillBox(level, x + 2, y + 1, z - 1, x + 4, y + 2, z + 1, 0);
	FillBox(level, x + 2, y + 3, z - 1, x + 4, y + 3, z + 1, Tile::wood_Id, 0);

	for (int mastY = y + 1; mastY <= y + 7; ++mastY)
	{
		PlaceBlock(level, x + 3, mastY, z, Tile::quartzBlock_Id, QuartzBlockTile::TYPE_LINES_Y);
	}

	FillBox(level, x + 2, y + 5, z - 3, x + 4, y + 5, z + 3, Tile::cloth_Id, ClothTile::getTileDataForItemAuxValue(0));
	PlaceBlock(level, x + 1, y + 1, z, Tile::enderChest_Id);
	PlaceEndShipLoot(level, x + 4, y + 1, z);
}

void GenerateEndCityAndShip(Level *level, Random *random, int centerX, int baseY, int centerZ)
{
	int cityBaseY = baseY + 1;

	FillBox(level, centerX - 4, cityBaseY - 1, centerZ - 4, centerX + 4, cityBaseY - 1, centerZ + 4, Tile::whiteStone_Id);
	for (int dz = -3; dz <= 3; dz += 3)
	{
		for (int dx = -3; dx <= 3; dx += 3)
		{
			BuildSupportColumn(level, centerX + dx, cityBaseY - 2, centerZ + dz, Tile::quartzBlock_Id);
		}
	}

	HollowBox(level, centerX - 3, cityBaseY, centerZ - 3, centerX + 3, cityBaseY + 12, centerZ + 3, Tile::quartzBlock_Id);
	FillBox(level, centerX - 2, cityBaseY + 4, centerZ - 2, centerX + 2, cityBaseY + 4, centerZ + 2, Tile::quartzBlock_Id);
	FillBox(level, centerX - 2, cityBaseY + 8, centerZ - 2, centerX + 2, cityBaseY + 8, centerZ + 2, Tile::quartzBlock_Id);
	FillBox(level, centerX - 3, cityBaseY + 12, centerZ - 3, centerX + 3, cityBaseY + 12, centerZ + 3, Tile::quartzBlock_Id);
	FillBox(level, centerX - 1, cityBaseY + 1, centerZ - 3, centerX + 1, cityBaseY + 2, centerZ - 3, 0);

	PlaceBlock(level, centerX - 3, cityBaseY + 2, centerZ, Tile::lightGem_Id);
	PlaceBlock(level, centerX + 3, cityBaseY + 6, centerZ, Tile::lightGem_Id);
	PlaceBlock(level, centerX, cityBaseY + 10, centerZ + 3, Tile::lightGem_Id);
	PlaceBlock(level, centerX, cityBaseY + 1, centerZ, Tile::enderChest_Id);

	for (int bridgeX = centerX + 4; bridgeX <= centerX + 8; ++bridgeX)
	{
		PlaceBlock(level, bridgeX, cityBaseY + 9, centerZ, Tile::quartzBlock_Id);
	}

	if (random->nextInt(100) < 75)
	{
		GenerateEndShip(level, centerX + 9, cityBaseY + 8, centerZ);
	}
}
}

TheEndLevelRandomLevelSource::TheEndLevelRandomLevelSource(Level *level, __int64 seed)
{
	m_XZSize = END_LEVEL_MIN_WIDTH;

    this->level = level;

    random = new Random(seed);
	pprandom = new Random(seed);	// 4J added
    lperlinNoise1 = new PerlinNoise(random, 16);
    lperlinNoise2 = new PerlinNoise(random, 16);
    perlinNoise1 = new PerlinNoise(random, 8);

    scaleNoise = new PerlinNoise(random, 10);
    depthNoise = new PerlinNoise(random, 16);
}

TheEndLevelRandomLevelSource::~TheEndLevelRandomLevelSource()
{
	delete random;
	delete pprandom;
	delete lperlinNoise1;
	delete lperlinNoise2;
	delete perlinNoise1;
	delete scaleNoise;
	delete depthNoise;
}

void TheEndLevelRandomLevelSource::prepareHeights(int xOffs, int zOffs, byteArray blocks, BiomeArray biomes)
{
	doubleArray buffer;	// 4J - used to be declared with class level scope but tidying up for thread safety reasons

    int xChunks = 16 / CHUNK_WIDTH;

    int xSize = xChunks + 1;
    int ySize = Level::genDepth / CHUNK_HEIGHT + 1;
    int zSize = xChunks + 1;
    buffer = getHeights(buffer, xOffs * xChunks, 0, zOffs * xChunks, xSize, ySize, zSize);

    for (int xc = 0; xc < xChunks; xc++)
	{
        for (int zc = 0; zc < xChunks; zc++)
		{
            for (int yc = 0; yc < Level::genDepth / CHUNK_HEIGHT; yc++)
			{
                double yStep = 1 / (double) CHUNK_HEIGHT;
                double s0 = buffer[((xc + 0) * zSize + (zc + 0)) * ySize + (yc + 0)];
                double s1 = buffer[((xc + 0) * zSize + (zc + 1)) * ySize + (yc + 0)];
                double s2 = buffer[((xc + 1) * zSize + (zc + 0)) * ySize + (yc + 0)];
                double s3 = buffer[((xc + 1) * zSize + (zc + 1)) * ySize + (yc + 0)];

                double s0a = (buffer[((xc + 0) * zSize + (zc + 0)) * ySize + (yc + 1)] - s0) * yStep;
                double s1a = (buffer[((xc + 0) * zSize + (zc + 1)) * ySize + (yc + 1)] - s1) * yStep;
                double s2a = (buffer[((xc + 1) * zSize + (zc + 0)) * ySize + (yc + 1)] - s2) * yStep;
                double s3a = (buffer[((xc + 1) * zSize + (zc + 1)) * ySize + (yc + 1)] - s3) * yStep;

                for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
                    double xStep = 1 / (double) CHUNK_WIDTH;

                    double _s0 = s0;
                    double _s1 = s1;
                    double _s0a = (s2 - s0) * xStep;
                    double _s1a = (s3 - s1) * xStep;

                    for (int x = 0; x < CHUNK_WIDTH; x++)
					{
                        int offs = (x + xc * CHUNK_WIDTH) << Level::genDepthBitsPlusFour | (0 + zc * CHUNK_WIDTH) << Level::genDepthBits | (yc * CHUNK_HEIGHT + y);
                        int step = 1 << Level::genDepthBits;
                        double zStep = 1 / (double) CHUNK_WIDTH;

                        double val = _s0;
                        double vala = (_s1 - _s0) * zStep;
                        for (int z = 0; z < CHUNK_WIDTH; z++)
						{
                            int tileId = 0;
                            if (val > 0)
							{
                                tileId = Tile::whiteStone_Id;
                            } else {
                            }

                            blocks[offs] = (byte) tileId;
                            offs += step;
                            val += vala;
                        }
                        _s0 += _s0a;
                        _s1 += _s1a;
                    }

                    s0 += s0a;
                    s1 += s1a;
                    s2 += s2a;
                    s3 += s3a;
                }
            }
        }
    }
	delete [] buffer.data;

}

void TheEndLevelRandomLevelSource::buildSurfaces(int xOffs, int zOffs, byteArray blocks, BiomeArray biomes)
{
    for (int x = 0; x < 16; x++)
	{
        for (int z = 0; z < 16; z++)
		{
            int runDepth = 1;
            int run = -1;

            byte top = (byte) Tile::whiteStone_Id;
            byte material = (byte) Tile::whiteStone_Id;

            for (int y = Level::genDepthMinusOne; y >= 0; y--)
			{
                int offs = (z * 16 + x) * Level::genDepth + y;

                int old = blocks[offs];

                if (old == 0)
				{
                    run = -1;
                }
				else if (old == Tile::rock_Id)
				{
                    if (run == -1)
					{
                        if (runDepth <= 0)
						{
                            top = 0;
                            material = (byte) Tile::whiteStone_Id;
                        }

                        run = runDepth;
                        if (y >= 0) blocks[offs] = top;
                        else blocks[offs] = material;
                    }
					else if (run > 0)
					{
                        run--;
                        blocks[offs] = material;
                    }
                }
            }
        }
    }
}

LevelChunk *TheEndLevelRandomLevelSource::create(int x, int z)
{
	return getChunk(x, z);
}

LevelChunk *TheEndLevelRandomLevelSource::getChunk(int xOffs, int zOffs)
{
    random->setSeed(xOffs * 341873128712l + zOffs * 132897987541l);

	BiomeArray biomes;
	// 4J - now allocating this with a physical alloc & bypassing general memory management so that it will get cleanly freed
	unsigned int blocksSize = Level::genDepth * 16 * 16;
	byte *tileData = (byte *)XPhysicalAlloc(blocksSize, MAXULONG_PTR, 4096, PAGE_READWRITE);
	XMemSet128(tileData,0,blocksSize);
	byteArray blocks = byteArray(tileData,blocksSize);
//    byteArray blocks = byteArray(16 * level->depth * 16);

//    LevelChunk *levelChunk = new LevelChunk(level, blocks, xOffs, zOffs);			// 4J moved below
    level->getBiomeSource()->getBiomeBlock(biomes, xOffs * 16, zOffs * 16, 16, 16, true);

    prepareHeights(xOffs, zOffs, blocks, biomes);
    buildSurfaces(xOffs, zOffs, blocks, biomes);

	// 4J - this now creates compressed block data from the blocks array passed in, so moved it until after the blocks are actually finalised. We also
	// now need to free the passed in blocks as the LevelChunk doesn't use the passed in allocation anymore.
	LevelChunk *levelChunk = new LevelChunk(level, blocks, xOffs, zOffs);
	XPhysicalFree(tileData);

    levelChunk->recalcHeightmap();

	//delete blocks.data; // Don't delete the blocks as the array data is actually owned by the chunk now
	delete biomes.data;

    return levelChunk;
}

doubleArray TheEndLevelRandomLevelSource::getHeights(doubleArray buffer, int x, int y, int z, int xSize, int ySize, int zSize)
{
    if (buffer.data == NULL)
	{
        buffer = doubleArray(xSize * ySize * zSize);
    }

    double s = 1 * 684.412;
    double hs = 1 * 684.412;

	doubleArray pnr, ar, br, sr, dr, fi, fis;	// 4J - used to be declared with class level scope but moved here for thread safety

    sr = scaleNoise->getRegion(sr, x, z, xSize, zSize, 1.121, 1.121, 0.5);
    dr = depthNoise->getRegion(dr, x, z, xSize, zSize, 200.0, 200.0, 0.5);

    s *= 2;

    pnr = perlinNoise1->getRegion(pnr, x, y, z, xSize, ySize, zSize, s / 80.0, hs / 160.0, s / 80.0);
    ar = lperlinNoise1->getRegion(ar, x, y, z, xSize, ySize, zSize, s, hs, s);
    br = lperlinNoise2->getRegion(br, x, y, z, xSize, ySize, zSize, s, hs, s);

    int p = 0;
    int pp = 0;

    for (int xx = 0; xx < xSize; xx++)
	{
        for (int zz = 0; zz < zSize; zz++)
		{
            double scale = ((sr[pp] + 256.0) / 512);
            if (scale > 1) scale = 1;


            double depth = (dr[pp] / 8000.0);
            if (depth < 0) depth = -depth * 0.3;
            depth = depth * 3.0 - 2.0;

            float xd = ((xx + x) - 0) / 1.0f;
            float zd = ((zz + z) - 0) / 1.0f;
            float dist = sqrt(xd * xd + zd * zd);
            float doffs = 100 - dist * 8;
            if (dist > 64.0f)
			{
				double ringDistance = abs(dist - 96.0f);
				double outerRing = 28.0 - ringDistance * 1.35;
				double islandSignal = abs(sr[pp]) * 0.05 + abs(dr[pp]) / 400.0;
				double outerDoffs = outerRing + islandSignal - 18.0;
				if (outerDoffs > doffs)
				{
					doffs = (float)outerDoffs;
				}
			}
            if (doffs > 80) doffs = 80;
            if (doffs < -100) doffs = -100;
            if (depth > 1) depth = 1;
            depth = depth / 8;
            depth = 0;

            if (scale < 0) scale = 0;
            scale = (scale) + 0.5;
            depth = depth * ySize / 16;

            pp++;

            double yCenter = ySize / 2.0;


            for (int yy = 0; yy < ySize; yy++)
			{
                double val = 0;
                double yOffs = (yy - (yCenter)) * 8 / scale;

                if (yOffs < 0) yOffs *= -1;

                double bb = ar[p] / 512;
                double cc = br[p] / 512;

                double v = (pnr[p] / 10 + 1) / 2;
                if (v < 0) val = bb;
                else if (v > 1) val = cc;
                else val = bb + (cc - bb) * v;
                val -= 8;
                val += doffs;

                int r = 2;
                if (yy > ySize / 2 - r)
				{
                    double slide = (yy - (ySize / 2 - r)) / (64.0f);
                    if (slide < 0) slide = 0;
                    if (slide > 1) slide = 1;
                    val = val * (1 - slide) + -3000 * slide;
                }
                r = 8;
                if (yy < r)
				{
                    double slide = (r - yy) / (r - 1.0f);
                    val = val * (1 - slide) + -30 * slide;
                }


                buffer[p] = val;
                p++;
            }
        }
    }

	delete [] pnr.data;
	delete [] ar.data;
	delete [] br.data;
	delete [] sr.data;
	delete [] dr.data;
	delete [] fi.data;
	delete [] fis.data;

    return buffer;

}

bool TheEndLevelRandomLevelSource::hasChunk(int x, int y)
{
	return true;
}

void TheEndLevelRandomLevelSource::calcWaterDepths(ChunkSource *parent, int xt, int zt)
{
    int xo = xt * 16;
    int zo = zt * 16;
    for (int x = 0; x < 16; x++)
	{
        int y = level->getSeaLevel();
        for (int z = 0; z < 16; z++)
		{
            int xp = xo + x + 7;
            int zp = zo + z + 7;
            int h = level->getHeightmap(xp, zp);
            if (h <= 0)
			{
                if (level->getHeightmap(xp - 1, zp) > 0 || level->getHeightmap(xp + 1, zp) > 0 || level->getHeightmap(xp, zp - 1) > 0 || level->getHeightmap(xp, zp + 1) > 0)
				{
                    bool hadWater = false;
                    if (hadWater || (level->getTile(xp - 1, y, zp) == Tile::calmWater_Id && level->getData(xp - 1, y, zp) < 7)) hadWater = true;
                    if (hadWater || (level->getTile(xp + 1, y, zp) == Tile::calmWater_Id && level->getData(xp + 1, y, zp) < 7)) hadWater = true;
                    if (hadWater || (level->getTile(xp, y, zp - 1) == Tile::calmWater_Id && level->getData(xp, y, zp - 1) < 7)) hadWater = true;
                    if (hadWater || (level->getTile(xp, y, zp + 1) == Tile::calmWater_Id && level->getData(xp, y, zp + 1) < 7)) hadWater = true;
                    if (hadWater)
					{
                        for (int x2 = -5; x2 <= 5; x2++)
						{
                            for (int z2 = -5; z2 <= 5; z2++)
							{
                                int d = (x2 > 0 ? x2 : -x2) + (z2 > 0 ? z2 : -z2);

                                if (d <= 5)
								{
                                    d = 6 - d;
                                    if (level->getTile(xp + x2, y, zp + z2) == Tile::calmWater_Id)
									{
                                        int od = level->getData(xp + x2, y, zp + z2);
                                        if (od < 7 && od < d)
										{
                                            level->setData(xp + x2, y, zp + z2, d);
                                        }
                                    }
                                }
                            }
                        }
                        if (hadWater)
						{
                            level->setTileAndDataNoUpdate(xp, y, zp, Tile::calmWater_Id, 7);
                            for (int y2 = 0; y2 < y; y2++)
							{
                                level->setTileAndDataNoUpdate(xp, y2, zp, Tile::calmWater_Id, 8);
                            }
                        }
                    }
                }
            }
        }
    }

}

void TheEndLevelRandomLevelSource::postProcess(ChunkSource *parent, int xt, int zt)
{
    HeavyTile::instaFall = true;
    int xo = xt * 16;
    int zo = zt * 16;

	// 4J - added. The original java didn't do any setting of the random seed here, and passes the level random to the biome decorator.
	// We'll be running our postProcess in parallel with getChunk etc. so we need to use a separate random - have used the same initialisation code as
	// used in RandomLevelSource::postProcess to make sure this random value is consistent for each world generation. 
	pprandom->setSeed(level->getSeed());
	__int64 xScale = pprandom->nextLong() / 2 * 2 + 1;
	__int64 zScale = pprandom->nextLong() / 2 * 2 + 1;
	pprandom->setSeed(((xt * xScale) + (zt * zScale)) ^ level->getSeed());

    Biome *biome = level->getBiome(xo + 16, zo + 16);
    biome->decorate(level, pprandom, xo, zo);		// 4J - passing pprandom rather than level->random here to make this consistent with our parallel world generation

	float distChunks = sqrt((float)(xt * xt + zt * zt));
	if (distChunks > 72.0f && distChunks < 220.0f && IsEndStructureChunk(level, xt, zt, 28, 5, 0x454E44))
	{
		int centerX = xo + 8;
		int centerZ = zo + 8;
		int groundY = FindEndSurfaceY(level, centerX, centerZ);
		if (groundY > 40 && HasEndFoundation(level, centerX, centerZ))
		{
			Random cityRandom;
			cityRandom.setSeed((((__int64)xt * 341873128712LL) ^ ((__int64)zt * 132897987541LL) ^ level->getSeed()) + 0xC171);
			GenerateEndCityAndShip(level, &cityRandom, centerX, groundY, centerZ);
		}
	}

    HeavyTile::instaFall = false;
	
	app.processSchematics(parent->getChunk(xt,zt));
}

bool TheEndLevelRandomLevelSource::save(bool force, ProgressListener *progressListener)
{
	return true;
}

bool TheEndLevelRandomLevelSource::tick()
{
	return false;
}

bool TheEndLevelRandomLevelSource::shouldSave()
{
	return true;
}

wstring TheEndLevelRandomLevelSource::gatherStats()
{
	return L"RandomLevelSource";
}

vector<Biome::MobSpawnerData *> *TheEndLevelRandomLevelSource::getMobsAt(MobCategory *mobCategory, int x, int y, int z)
{
    Biome *biome = level->getBiome(x, z);
    if (biome == NULL)
	{
        return NULL;
    }
    return biome->getMobs(mobCategory);
}

TilePos *TheEndLevelRandomLevelSource::findNearestMapFeature(Level *level, const wstring& featureName, int x, int y, int z)
{
	return NULL;
}
 
