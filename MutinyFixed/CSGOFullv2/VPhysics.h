#pragma once

class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps(void) {}
	virtual int             ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	virtual int             SurfacePropCount(void) const = 0;
	virtual int             GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void            GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;
	virtual surfacedata_t*  GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char*     GetString(unsigned short stringTableIndex) const = 0;
	virtual const char*     GetPropName(int surfaceDataIndex) const = 0;
	virtual void            SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;
	virtual void            GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t *pParamsOut) const = 0;
};
