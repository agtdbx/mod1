#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>
#include <engine/render/shader/ShaderFunctions.hpp>

//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::computePredictedPositions(
			ShaderManager *shaderManager,
			float delta)
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
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveFloatToShader(shaderId, "waterMaxXZ", WATER_MAX_XZ);
	giveFloatToShader(shaderId, "waterMaxY", WATER_MAX_HEIGHT);
	giveFloatToShader(shaderId, "gravityForce", GRAVITY_FORCE);
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
	// int	gx, gy, gz, gid;

	// // Buffer to vector
	// this->predictedPositionsFromBuffer();
	// // Clear grid
	// for (int i = 0; i < this->gridSize; i++)
	// 	this->grid[i].clear();

	// // Put particles into grid
	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	gx = this->predictedPositions[i].x / SMOOTHING_RADIUS;
	// 	gy = this->predictedPositions[i].y / SMOOTHING_RADIUS;
	// 	gz = this->predictedPositions[i].z / SMOOTHING_RADIUS;

	// 	gid = gx + gz * this->gridW + gy * this->idHsize;
	// 	this->grid[gid].push_back(i);
	// }

	// // Clear render grid
	// for (int i = 0; i < this->renderGridSize; i++)
	// 	this->renderGrid[i].clear();

	// // Put particles into render grid
	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	gx = this->predictedPositions[i].x / RENDER_CELL_SIZE;
	// 	gy = this->predictedPositions[i].y / RENDER_CELL_SIZE;
	// 	gz = this->predictedPositions[i].z / RENDER_CELL_SIZE;

	// 	gid = gx + gz * this->renderGridW + gy * this->renderIdHsize;
	// 	this->renderGrid[gid].push_back(i);
	// }

	// this->generateFlatGrid();
	// this->gridFlatToBuffer();
	// this->gridOffsetsToBuffer();
	// this->renderGridFlatToBuffer();
	// this->renderGridOffsetsToBuffer();

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
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridFlatSize);
	giveIntToShader(shaderId, "offsetsSize", this->gridOffsetsSize);
	giveIntToShader(shaderId, "positionsSize", this->nbParticules);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
								this->textureBufferPositions,
								this->texturePositions);

	// Compute shader output setup
	giveFloatTextureInputToShader(0, true, this->textureBufferGridOffsets,
									this->textureGridOffsets);
	giveFloatTextureInputToShader(1, true, this->textureBufferGridFlat,
									this->textureGridFlat);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
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
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingScale", SMOOTHING_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridFlatSize);
	giveIntToShader(shaderId, "offsetsSize", this->gridOffsetsSize);
	giveFloatTextureToShader(shaderId, "gridBuffer", 1,
								this->textureBufferGridFlat,
								this->textureGridFlat);
	giveFloatTextureToShader(shaderId, "offsetsBuffer", 2,
								this->textureBufferGridOffsets,
								this->textureGridOffsets);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 3,
								this->textureBufferPositions,
								this->texturePositions);

	// Compute shader output setup
	giveFloatTextureInputToShader(0, false, this->textureBufferDensities,
									this->textureDensities);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
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
	giveFloatToShader(shaderId, "smoothingScale", SMOOTHING_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);
	giveIntToShader(shaderId, "mapDensityCellSize", MAP_DENSITY_CELL_SIZE);
	giveIntToShader(shaderId, "mapDensityW", this->mapDensityW);
	giveIntToShader(shaderId, "mapDensityIdHsize", this->mapDensityIdHsize);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridFlatSize);
	giveIntToShader(shaderId, "offsetsSize", this->gridOffsetsSize);
	giveFloatTextureToShader(shaderId, "gridBuffer", 1,
								this->textureBufferGridFlat,
								this->textureGridFlat);
	giveFloatTextureToShader(shaderId, "offsetsBuffer", 2,
								this->textureBufferGridOffsets,
								this->textureGridOffsets);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 3,
								this->textureBufferPositions,
								this->texturePositions);

	// Compute shader output setup
	giveFloatTextureInputToShader(0, false, this->textureBufferMapDensities,
									this->textureMapDensities);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroupsMapDensity, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::calculatesAndApplyPressure(
			ShaderManager *shaderManager,
			float delta)
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
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS);
	giveFloatToShader(shaderId, "smoothingDerivateScale",
						SMOOTHING_DERIVATE_SCALE);
	giveFloatToShader(shaderId, "smoothingViscosityScale",
						SMOOTHING_VISCOSITY_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);
	giveFloatToShader(shaderId, "waterRadius2", WATER_RADIUS2);
	giveFloatToShader(shaderId, "targetDensity", TARGET_DENSITY);
	giveFloatToShader(shaderId, "pressureMultiplier", PRESSURE_MULTIPLIER);
	giveFloatToShader(shaderId, "viscosityStrength", VISCOSITY_FORCE);
	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridFlatSize);
	giveIntToShader(shaderId, "offsetsSize", this->gridOffsetsSize);
	giveFloatTextureToShader(shaderId, "gridBuffer", 1,
								this->textureBufferGridFlat,
								this->textureGridFlat);
	giveFloatTextureToShader(shaderId, "offsetsBuffer", 2,
								this->textureBufferGridOffsets,
								this->textureGridOffsets);
	giveVec4TextureToShader(shaderId, "predictedPositionsBuffer", 3,
								this->textureBufferPredictedPositions,
								this->texturePredictedPositions);
	giveFloatTextureToShader(shaderId, "densitiesBuffer", 4,
								this->textureBufferDensities,
								this->textureDensities);

	// Compute shader output setup
	giveVec4TextureInputToShader(0, true, this->textureBufferVelocities,
									this->textureVelocities);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::updatePositions(
							ShaderManager *shaderManager,
							Terrain *terrain,
							float delta)
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
	giveFloatToShader(shaderId, "delta", delta);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveFloatToShader(shaderId, "waterMaxXZ", WATER_MAX_XZ);
	giveFloatToShader(shaderId, "waterMaxY", WATER_MAX_HEIGHT);
	giveFloatToShader(shaderId, "collisionEnergyKeep", COLLISION_ENERGY_KEEP);
	giveFloatToShader(shaderId, "collisionRepulsionForce",
						COLLISION_REPULSION_FORCE);
	giveFloatToShader(shaderId, "terrainCellSize", TERRAIN_CELL_SIZE);
	giveIntToShader(shaderId, "terrainGridW", sizes[0]);
	giveIntToShader(shaderId, "terrainGridH", sizes[1]);
	giveIntToShader(shaderId, "terrainGridD", sizes[2]);
	giveIntToShader(shaderId, "terrainIdHsize", sizes[3]);
	giveIntToShader(shaderId, "terrainGridSize", terrainFlatGridSize);
	giveIntToShader(shaderId, "terrainOffsetsSize", terrainOffsetsSize);
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
