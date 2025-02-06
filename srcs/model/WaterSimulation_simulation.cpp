#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>
#include <engine/render/shader/ShaderFunctions.hpp>

//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::computePredictedPositions(
			ShaderManager *shaderManager,
			float delta,
			t_holeInfo *holeInfo)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("predictedPositions");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveBoolToShader(shaderId, "holeEnable", holeInfo->enable);
	giveFloatToShader(shaderId, "holeRadius2", holeInfo->radius2);
	giveVec2ToShader(shaderId, "holePosition", holeInfo->position);
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveFloatToShader(shaderId, "waterMaxXZ", WATER_MAX_XZ);
	giveFloatToShader(shaderId, "waterMaxY", WATER_MAX_HEIGHT);
	giveFloatToShader(shaderId, "gravityForce", GRAVITY_FORCE);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
							this->textureBufferPositions,
							this->texturePositions);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, false, this->textureBufferPredictedPositions,
									this->texturePredictedPositions);
	giveVec4TextureInputToShader(1, true, this->textureBufferVelocities,
									this->textureVelocities);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::putParticlesInGrid(
			ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("putInGrid");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "invSmoothingRadius", INV_SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridSize);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 3,
								this->textureBufferPositions,
								this->texturePositions);

	// Compute shader output setup
	giveSSBBOInputToShader(0, this->ssboGrid1);
	this->ssboCurrent = this->ssboGrid1;
	this->ssboGrid1Used = true;

	// Run compute shader
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::putParticlesInGridInParallel(
			ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("putInGridParallel");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();
	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridSize);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);

	// Compute shader output setup
	if (this->ssboGrid1Used)
	{
		giveSSBBOInputToShader(0, this->ssboGrid1);
		giveSSBBOInputToShader(1, this->ssboGrid2);
		this->ssboCurrent = this->ssboGrid2;
		this->ssboGrid1Used = false;
	}
	else
	{
		giveSSBBOInputToShader(0, this->ssboGrid2);
		giveSSBBOInputToShader(1, this->ssboGrid1);
		this->ssboCurrent = this->ssboGrid1;
		this->ssboGrid1Used = true;
	}

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroupsPutInGrid, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::computeMapDensity(ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("mapDensities");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingRadius2", SMOOTHING_RADIUS2);
	giveFloatToShader(shaderId, "invSmoothingRadius", INV_SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingScale", SMOOTHING_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);
	giveFloatToShader(shaderId, "targetDensity", TARGET_DENSITY);
	giveFloatToShader(shaderId, "pressureMultiplier", PRESSURE_MULTIPLIER);
	giveIntToShader(shaderId, "mapBufferCellSize", MAP_BUFFER_CELL_SIZE);
	giveIntToShader(shaderId, "mapBufferW", this->mapBufferW);
	giveIntToShader(shaderId, "mapBufferIdHsize", this->mapBufferIdHsize);
	giveIntToShader(shaderId, "mapBufferSize", this->mapBufferSize);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridSize);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);

	// Compute shader output setup
	giveFloatTextureInputToShader(0, false, this->textureBufferMapDensities,
									this->textureMapDensities);
	giveSSBBOInputToShader(1, this->ssboCurrent);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroupsMapBuffer, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::computeDensity(
			ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("densities");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "targetDensity", TARGET_DENSITY);
	giveFloatToShader(shaderId, "pressureMultiplier", PRESSURE_MULTIPLIER);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 3,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);

	giveIntToShader(shaderId, "mapBufferCellSize", MAP_BUFFER_CELL_SIZE);
	giveIntToShader(shaderId, "mapBufferW", this->mapBufferW);
	giveIntToShader(shaderId, "mapBufferH", this->mapBufferH);
	giveIntToShader(shaderId, "mapBufferD", this->mapBufferD);
	giveIntToShader(shaderId, "mapBufferIdHsize", this->mapBufferIdHsize);
	giveFloatTextureToShader(shaderId, "mapDensitiesBuffer", 4,
								this->textureBufferMapDensities,
								this->textureMapDensities);

	// Compute shader output setup
	giveFloatTextureInputToShader(0, false, this->textureBufferDensities,
									this->textureDensities);
	giveFloatTextureInputToShader(1, false, this->textureBufferPressures,
									this->texturePressures);
	giveSSBBOInputToShader(2, this->ssboCurrent);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::calculatesAndApplyPressure(
			ShaderManager *shaderManager,
			float delta,
			t_holeInfo *holeInfo)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("pressure");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveBoolToShader(shaderId, "holeEnable", holeInfo->enable);
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "invSmoothingRadius", INV_SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingDerivateScale",
						SMOOTHING_DERIVATE_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridSize);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "predictedPositionsBuffer", 2,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);
	giveFloatTextureToShader(shaderId, "densitiesBuffer", 3,
								this->textureBufferDensities,
								this->textureDensities);
	giveFloatTextureToShader(shaderId, "pressuresBuffer", 4,
								this->textureBufferPressures,
								this->texturePressures);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, true, this->textureBufferVelocities,
									this->textureVelocities);
	giveSSBBOInputToShader(1, this->ssboCurrent);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::computeMapViscosity(ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("mapViscosity");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingRadius2", SMOOTHING_RADIUS2);
	giveFloatToShader(shaderId, "invSmoothingRadius", INV_SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingScale", SMOOTHING_SCALE);
	giveFloatToShader(shaderId, "smoothingViscosityScale",
						SMOOTHING_VISCOSITY_SCALE);
	giveFloatToShader(shaderId, "waterRadius2", WATER_RADIUS2);
	giveFloatToShader(shaderId, "viscosityStrength", VISCOSITY_FORCE);
	giveIntToShader(shaderId, "mapBufferCellSize", MAP_BUFFER_CELL_SIZE);
	giveIntToShader(shaderId, "mapBufferW", this->mapBufferW);
	giveIntToShader(shaderId, "mapBufferIdHsize", this->mapBufferIdHsize);
	giveIntToShader(shaderId, "mapBufferSize", this->mapBufferSize);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridSize);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);
	giveVec4TextureToShader(shaderId, "velocitiesBuffer", 3,
								this->textureBufferVelocities,
								this->textureVelocities);
	giveFloatTextureToShader(shaderId, "densitiesBuffer", 4,
								this->textureBufferDensities,
								this->textureDensities);
	giveFloatTextureToShader(shaderId, "mapDensitiesBuffer", 5,
								this->textureBufferMapDensities,
								this->textureMapDensities);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, true, this->textureBufferMapViscosities,
									this->textureMapViscosities);
	giveSSBBOInputToShader(1, this->ssboCurrent);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroupsMapBuffer, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::calculatesAndApplyViscosity(
			ShaderManager *shaderManager,
			float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("viscosity");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	giveFloatToShader(shaderId, "delta", delta);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "predictedPositionsBuffer", 2,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);
	giveFloatTextureToShader(shaderId, "densitiesBuffer", 3,
								this->textureBufferDensities,
								this->textureDensities);

	giveIntToShader(shaderId, "mapBufferCellSize", MAP_BUFFER_CELL_SIZE);
	giveIntToShader(shaderId, "mapBufferW", this->mapBufferW);
	giveIntToShader(shaderId, "mapBufferH", this->mapBufferH);
	giveIntToShader(shaderId, "mapBufferD", this->mapBufferD);
	giveIntToShader(shaderId, "mapBufferIdHsize", this->mapBufferIdHsize);
	giveVec4TextureToShader(shaderId, "mapViscositiesBuffer", 4,
								this->textureBufferMapViscosities,
								this->textureMapViscosities);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, true, this->textureBufferVelocities,
									this->textureVelocities);
	giveSSBBOInputToShader(1, this->ssboCurrent);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::updatePositions(
							ShaderManager *shaderManager,
							Terrain *terrain,
							float delta,
							t_holeInfo *holeInfo)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;
	int				sizes[4], terrainFlatGridSize, terrainOffsetsSize;
	GLuint			terrainBufferTextureDataGrid, terrainTextureDataGrid,
					terrainBufferTextureFlatGrid, terrainTextureFlatGrid,
					terrainBufferTextureOffsets, terrainTextureOffsets;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("updatePositions");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Get terrain data
	terrainBufferTextureDataGrid = terrain->getTextureBufferTerrainGridData();
	terrainTextureDataGrid = terrain->getTextureTerrainGridData();
	terrainBufferTextureFlatGrid = terrain->getTextureBufferTerrainGridFlat();
	terrainTextureFlatGrid = terrain->getTextureTerrainGridFlat();
	terrainFlatGridSize = terrain->getSizeTerrainGridFlat();
	terrainBufferTextureOffsets = terrain->getTextureBufferTerrainGridOffsets();
	terrainTextureOffsets = terrain->getTextureTerrainGridOffsets();
	terrainOffsetsSize = terrain->getSizeTerrainGridOffsets();
	terrain->getGridSize(sizes);

	// Compute shader inputs setup
	giveBoolToShader(shaderId, "holeEnable", holeInfo->enable);
	giveFloatToShader(shaderId, "holeRadius2", holeInfo->radius2);
	giveVec2ToShader(shaderId, "holePosition", holeInfo->position);
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveFloatToShader(shaderId, "waterMaxXZ", WATER_MAX_XZ);
	giveFloatToShader(shaderId, "waterMaxY", WATER_MAX_HEIGHT);
	giveFloatToShader(shaderId, "collisionEnergyKeep", COLLISION_ENERGY_KEEP);
	giveFloatToShader(shaderId, "collisionRepulsionForce",
						COLLISION_REPULSION_FORCE);
	giveFloatToShader(shaderId, "invTerrainCellSize", INV_TERRAIN_CELL_SIZE);
	giveIntToShader(shaderId, "terrainGridW", sizes[0]);
	giveIntToShader(shaderId, "terrainGridH", sizes[1]);
	giveIntToShader(shaderId, "terrainGridD", sizes[2]);
	giveIntToShader(shaderId, "terrainIdHsize", sizes[3]);
	giveIntToShader(shaderId, "terrainGridSize", terrainFlatGridSize);
	giveIntToShader(shaderId, "terrainOffsetsSize", terrainOffsetsSize);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec3TextureToShader(shaderId, "terrainDataBuffer", 2,
								terrainBufferTextureDataGrid,
								terrainTextureDataGrid);
	giveFloatTextureToShader(shaderId, "terrainGridBuffer", 3,
								terrainBufferTextureFlatGrid,
								terrainTextureFlatGrid);
	giveFloatTextureToShader(shaderId, "terrainOffsetsBuffer", 4,
								terrainBufferTextureOffsets,
								terrainTextureOffsets);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, false, this->textureBufferPositions,
									this->texturePositions);
	giveVec4TextureInputToShader(1, false, this->textureBufferVelocities,
									this->textureVelocities);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
