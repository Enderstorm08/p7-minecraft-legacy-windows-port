#include "stdafx.h"

#include "DerivedLevelData.h"

DerivedLevelData::DerivedLevelData(LevelData *wrapped)
{
	this->wrapped = wrapped;
}

void DerivedLevelData::setTagData(CompoundTag *tag)
{
	wrapped->setTagData(tag);
}

CompoundTag *DerivedLevelData::createTag()
{
	return wrapped->createTag();
}

CompoundTag *DerivedLevelData::createTag(vector<shared_ptr<Player> > *players)
{
	return wrapped->createTag(players);
}

__int64 DerivedLevelData::getSeed()
{
	return wrapped->getSeed();
}

int DerivedLevelData::getXSpawn()
{
	return wrapped->getXSpawn();
}


int DerivedLevelData::getYSpawn()
{
	return wrapped->getYSpawn();
}

int DerivedLevelData::getZSpawn()
{
	return wrapped->getZSpawn();
}

int DerivedLevelData::getXStronghold()
{
	return wrapped->getXStronghold();
}

int DerivedLevelData::getZStronghold()
{
	return wrapped->getZStronghold();
}

int DerivedLevelData::getXStrongholdEndPortal()
{
	return wrapped->getXStrongholdEndPortal();
}

int DerivedLevelData::getZStrongholdEndPortal()
{
	return wrapped->getZStrongholdEndPortal();
}

bool DerivedLevelData::getHasStronghold()
{
	return wrapped->getHasStronghold();
}

bool DerivedLevelData::getHasStrongholdEndPortal()
{
	return wrapped->getHasStrongholdEndPortal();
}

bool DerivedLevelData::getHasEndGateway()
{
	return wrapped->getHasEndGateway();
}

int DerivedLevelData::getXEndGateway()
{
	return wrapped->getXEndGateway();
}

int DerivedLevelData::getYEndGateway()
{
	return wrapped->getYEndGateway();
}

int DerivedLevelData::getZEndGateway()
{
	return wrapped->getZEndGateway();
}

int DerivedLevelData::getXOuterEndGateway()
{
	return wrapped->getXOuterEndGateway();
}

int DerivedLevelData::getYOuterEndGateway()
{
	return wrapped->getYOuterEndGateway();
}

int DerivedLevelData::getZOuterEndGateway()
{
	return wrapped->getZOuterEndGateway();
}

__int64 DerivedLevelData::getTime()
{
	return wrapped->getTime();
}

__int64 DerivedLevelData::getSizeOnDisk()
{
	return wrapped->getSizeOnDisk();
}

CompoundTag *DerivedLevelData::getLoadedPlayerTag()
{
	return wrapped->getLoadedPlayerTag();
}

wstring DerivedLevelData::getLevelName()
{
	return wrapped->getLevelName();
}

int DerivedLevelData::getVersion()
{
	return wrapped->getVersion();
}

__int64 DerivedLevelData::getLastPlayed()
{
	return wrapped->getLastPlayed();
}

bool DerivedLevelData::isThundering()
{
	return wrapped->isThundering();
}

int DerivedLevelData::getThunderTime()
{
	return wrapped->getThunderTime();
}

bool DerivedLevelData::isRaining()
{
	return wrapped->isRaining();
}

int DerivedLevelData::getRainTime()
{
	return wrapped->getRainTime();
}

GameType *DerivedLevelData::getGameType()
{
	return wrapped->getGameType();
}

void DerivedLevelData::setSeed(__int64 seed)
{
}

void DerivedLevelData::setXSpawn(int xSpawn)
{
}

void DerivedLevelData::setYSpawn(int ySpawn)
{
}

void DerivedLevelData::setZSpawn(int zSpawn)
{
}

void DerivedLevelData::setHasStronghold()
{
	wrapped->setHasStronghold();
}

void DerivedLevelData::setXStronghold(int xStronghold)
{
	wrapped->setXStronghold(xStronghold);
}

void DerivedLevelData::setZStronghold(int zStronghold)
{
	wrapped->setZStronghold(zStronghold);
}

void DerivedLevelData::setHasStrongholdEndPortal()
{
	wrapped->setHasStrongholdEndPortal();
}

void DerivedLevelData::setXStrongholdEndPortal(int xStrongholdEndPortal)
{
	wrapped->setXStrongholdEndPortal(xStrongholdEndPortal);
}

void DerivedLevelData::setZStrongholdEndPortal(int zStrongholdEndPortal)
{
	wrapped->setZStrongholdEndPortal(zStrongholdEndPortal);
}

void DerivedLevelData::setHasEndGateway(bool hasEndGateway)
{
	wrapped->setHasEndGateway(hasEndGateway);
}

void DerivedLevelData::setXEndGateway(int xEndGateway)
{
	wrapped->setXEndGateway(xEndGateway);
}

void DerivedLevelData::setYEndGateway(int yEndGateway)
{
	wrapped->setYEndGateway(yEndGateway);
}

void DerivedLevelData::setZEndGateway(int zEndGateway)
{
	wrapped->setZEndGateway(zEndGateway);
}

void DerivedLevelData::setXOuterEndGateway(int xOuterEndGateway)
{
	wrapped->setXOuterEndGateway(xOuterEndGateway);
}

void DerivedLevelData::setYOuterEndGateway(int yOuterEndGateway)
{
	wrapped->setYOuterEndGateway(yOuterEndGateway);
}

void DerivedLevelData::setZOuterEndGateway(int zOuterEndGateway)
{
	wrapped->setZOuterEndGateway(zOuterEndGateway);
}

void DerivedLevelData::setTime(__int64 time)
{
}

void DerivedLevelData::setSizeOnDisk(__int64 sizeOnDisk)
{
}

void DerivedLevelData::setLoadedPlayerTag(CompoundTag *loadedPlayerTag)
{
}

void DerivedLevelData::setDimension(int dimension)
{
}

void DerivedLevelData::setSpawn(int xSpawn, int ySpawn, int zSpawn)
{
}

void DerivedLevelData::setLevelName(const wstring &levelName)
{
}

void DerivedLevelData::setVersion(int version)
{
}

void DerivedLevelData::setThundering(bool thundering)
{
}

void DerivedLevelData::setThunderTime(int thunderTime)
{
}

void DerivedLevelData::setRaining(bool raining)
{
}

void DerivedLevelData::setRainTime(int rainTime)
{
}

bool DerivedLevelData::isGenerateMapFeatures()
{
	return wrapped->isGenerateMapFeatures();
}

void DerivedLevelData::setGameType(GameType *gameType) {
}

bool DerivedLevelData::isHardcore()
{
	return wrapped->isHardcore();
}

LevelType *DerivedLevelData::getGenerator()
{
	return wrapped->getGenerator();
}

void DerivedLevelData::setGenerator(LevelType *generator)
{
}

bool DerivedLevelData::getAllowCommands()
{
	return wrapped->getAllowCommands();
}

void DerivedLevelData::setAllowCommands(bool allowCommands)
{
}

bool DerivedLevelData::isInitialized()
{
	return wrapped->isInitialized();
}

void DerivedLevelData::setInitialized(bool initialized)
{
}

int DerivedLevelData::getXZSize()
{
	return wrapped->getXZSize();
}

int DerivedLevelData::getHellScale()
{
	return wrapped->getHellScale();
}
