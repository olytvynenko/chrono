
/*
 * SetupFsiParams.h
 *
 *  Created on: Mar 2, 2015
 *      Author: Arman Pazouki
 */
#ifndef FSI_HMMWV_PARAMS_H_
#define FSI_HMMWV_PARAMS_H_

#include "SPHCudaUtils.h"
#include "MyStructs.cuh"  //just for SimParams
#include "VehicleExtraProperties.h"
#include <fstream> // for simParams definition


// -----------------------------------------------------------------------------
// Simulation parameters FSI
// -----------------------------------------------------------------------------

// Duration of the "hold time" (vehicle chassis fixed and no driver inputs).
// This can be used to allow the granular material to settle.
Real time_hold_vehicle = 1.0;//0.1;  // 0.2;
Real time_pause_fluid_external_force = 0;//.05;//0.1;//0.1;  // 0.2;

Real contact_recovery_speed = 1;
Real maxFlowVelocity = 12;  // in an ideal case, these two need to be the same

Real time_step = 2e-4;//0.2e-4;//1.0e-4;  // 2e-3;  // note you are using half of this for MBD system
// Total simulation duration.
Real time_end = 15;

// Dimensions
Real hdimX = 14;  // 5.5;
Real hdimY = 1.75;
Real hdimZ = 0.5;
Real hthick = 0.25;
Real basinDepth = 2;

Real fluidInitDimX = 2;
Real fluidInitDimY = hdimY;
Real fluidHeight = 1.4;// 2.0;

int fluidCollisionFamily = 1; // 2 and 3 are reserved for tire and chassis

// -----------------------------------------------------------------------------
// Simulation parameters Fluid
// -----------------------------------------------------------------------------
int tStepsCheckPoint = 1000;
bool initializeFluidFromFile = false; 	// 	IMPORTANT: when true, "haveFluid" in fsi_hmmwv.cpp should be true too.
										//	when adding functionality using "useWallBce" and "haveFluid" macros, pay attention to  "initializeFluidFromFile" options.
										//	for a double security, do your best to set "haveFluid" and "useWallBce" based on the data you have from checkpoint files
BceVersion bceType = ADAMI;//mORIGINAL; //ADAMI; // when set to adami, change the LARGE_PRES to zero. although not very important, since this option will overwrite the BCE pressure and
							// paramsH.LARGE_PRES is only used for the initialization of the BCE markers

NumberOfObjects numObjects;

void SetupParamsH(SimParams& paramsH) {
  //**********************************************
  paramsH.sizeScale = 1;  // don't change it.
  paramsH.HSML = 0.2;//0.04;
  paramsH.MULT_INITSPACE = 1.0;
  paramsH.NUM_BOUNDARY_LAYERS = 3;
  paramsH.toleranceZone = paramsH.NUM_BOUNDARY_LAYERS * (paramsH.HSML * paramsH.MULT_INITSPACE);
  paramsH.BASEPRES = 0;        // 10;
  paramsH.LARGE_PRES = 0;  // paramsH.BASEPRES;//10000;
  paramsH.deltaPress;          //** modified below
  paramsH.multViscosity_FSI = 5.0;
  paramsH.gravity = mR3(0, 0, -9.81);  // mR3(0);//mR3(0, -9.81, 0);
  paramsH.bodyForce3 =
      mR3(0, 0, 0);  // mR4(3.2e-3,0,0,0);// mR4(0);;// /*Re = 100 */ //mR4(3.2e-4, 0, 0, 0);/*Re = 100 */
  paramsH.rho0 = 1000;
  paramsH.mu0 = .001;
  paramsH.v_Max = maxFlowVelocity;  // Arman, I changed it to 0.1 for vehicle. Check this
                                    // later;//10;//50e-3;//18e-3;//1.5;//2e-1; /*0.2 for Re = 100 */ //2e-3;
  paramsH.EPS_XSPH = .5f;
  paramsH.dT = time_step;         // 0.0005;//0.1;//.001; //sph alone: .01 for Re 10;
  paramsH.tFinal = time_end;      // 20 * paramsH.dT; //400
  paramsH.timePause = time_pause_fluid_external_force;  //.0001 * paramsH.tFinal;//.0001 * paramsH.tFinal; 	// time before applying any
  // bodyforce. Particles move only due to initialization. keep it as small as possible.
  // the time step will be 1/10 * dT.
  paramsH.kdT = 5;  // I don't know what is kdT
  paramsH.gammaBB = 0.5;
  // ************
  paramsH.binSize0;           // will be changed
  paramsH.rigidRadius;        // will be changed
  paramsH.densityReinit = 1;  // 0: no re-initialization, 1: with initialization

  paramsH.enableTweak = 1 ; // 0: no tweak, 1: have tweak
  paramsH.enableAggressiveTweak = 1 ; // 0: no aggressive tweak; 1: with aggressive tweak (if 1, enableTweak should be 1 too)
  //********************************************************************************************************
  //**  reminiscent of the past******************************************************************************
  //	paramsH.cMin = mR3(-paramsH.toleranceZone, -paramsH.toleranceZone, -paramsH.toleranceZone);
  //// 3D channel
  //	paramsH.cMax = mR3( 3  + paramsH.toleranceZone, 2 + paramsH.toleranceZone,  3 + paramsH.toleranceZone);
  //  paramsH.cMin = mR3(0, 0, 0);  // 3D channel
  //  paramsH.cMax = mR3(3, 2, 3);
  paramsH.cMin = mR3(-hdimX, -hdimY, -basinDepth - hthick);  // 3D channel
  paramsH.cMax = mR3(hdimX, hdimY, basinDepth);
  //****************************************************************************************
  // printf("a1  paramsH.cMax.x, y, z %f %f %f,  binSize %f\n", paramsH.cMax.x, paramsH.cMax.y, paramsH.cMax.z, 2 *
  // paramsH.HSML);
  int3 side0 = mI3(floor((paramsH.cMax.x - paramsH.cMin.x) / (2 * paramsH.HSML)),
                   floor((paramsH.cMax.y - paramsH.cMin.y) / (2 * paramsH.HSML)),
                   floor((paramsH.cMax.z - paramsH.cMin.z) / (2 * paramsH.HSML)));
  Real3 binSize3 = mR3((paramsH.cMax.x - paramsH.cMin.x) / side0.x,
                       (paramsH.cMax.y - paramsH.cMin.y) / side0.y,
                       (paramsH.cMax.z - paramsH.cMin.z) / side0.z);
  paramsH.binSize0 = (binSize3.x > binSize3.y) ? binSize3.x : binSize3.y;
  //	paramsH.binSize0 = (paramsH.binSize0 > binSize3.z) ? paramsH.binSize0 : binSize3.z;
  paramsH.binSize0 =
      binSize3.x;  // for effect of distance. Periodic BC in x direction. we do not care about paramsH.cMax y and z.
  paramsH.cMax = paramsH.cMin + paramsH.binSize0 * mR3(side0);
  paramsH.boxDims = paramsH.cMax - paramsH.cMin;
  //****************************************************************************************
  paramsH.cMinInit = mR3(-fluidInitDimX, paramsH.cMin.y, -basinDepth + 1.0 * paramsH.HSML);  // 3D channel
  paramsH.cMaxInit = mR3(fluidInitDimX, paramsH.cMax.y, paramsH.cMinInit.z + fluidHeight);
  //****************************************************************************************
  //*** initialize straight channel
  paramsH.straightChannelBoundaryMin = paramsH.cMinInit;  // mR3(0, 0, 0);  // 3D channel
  paramsH.straightChannelBoundaryMax = paramsH.cMaxInit;  // SmR3(3, 2, 3) * paramsH.sizeScale;
  //************************** modify pressure ***************************
  //		paramsH.deltaPress = paramsH.rho0 * paramsH.boxDims * paramsH.bodyForce3;  //did not work as I expected
  paramsH.deltaPress = 0.9 * paramsH.boxDims * paramsH.bodyForce3;

  // modify bin size stuff
  //****************************** bin size adjustement and contact detection stuff *****************************
  int3 SIDE = mI3(int((paramsH.cMax.x - paramsH.cMin.x) / paramsH.binSize0 + .1),
                  int((paramsH.cMax.y - paramsH.cMin.y) / paramsH.binSize0 + .1),
                  int((paramsH.cMax.z - paramsH.cMin.z) / paramsH.binSize0 + .1));
  Real mBinSize = paramsH.binSize0;  // Best solution in that case may be to change cMax or cMin such that periodic
                                     // sides be a multiple of binSize
  //**********************************************************************************************************
  paramsH.gridSize = SIDE;
  // paramsH.numCells = SIDE.x * SIDE.y * SIDE.z;
  paramsH.worldOrigin = paramsH.cMin;
  paramsH.cellSize = mR3(mBinSize, mBinSize, mBinSize);

  //***** print numbers
  printf(
      "********************\n paramsH.sizeScale: %f\n paramsH.HSML: %f\n paramsH.bodyForce3: %f %f %f\n "
      "paramsH.gravity: %f %f %f\n paramsH.rho0: %e\n paramsH.mu0: %f\n paramsH.v_Max: %f\n paramsH.dT: %e\n "
      "paramsH.tFinal: %f\n  paramsH.timePause: %f\n  paramsH.timePauseRigidFlex: %f\n paramsH.densityReinit: %d\n",
      paramsH.sizeScale,
      paramsH.HSML,
      paramsH.bodyForce3.x,
      paramsH.bodyForce3.y,
      paramsH.bodyForce3.z,
      paramsH.gravity.x,
      paramsH.gravity.y,
      paramsH.gravity.z,
      paramsH.rho0,
      paramsH.mu0,
      paramsH.v_Max,
      paramsH.dT,
      paramsH.tFinal,
      paramsH.timePause,
      paramsH.timePauseRigidFlex,
      paramsH.densityReinit);
  printf(" paramsH.cMin: %f %f %f, paramsH.cMax: %f %f %f\n binSize: %f\n",
         paramsH.cMin.x,
         paramsH.cMin.y,
         paramsH.cMin.z,
         paramsH.cMax.x,
         paramsH.cMax.y,
         paramsH.cMax.z,
         paramsH.binSize0);
  printf(" paramsH.MULT_INITSPACE: %f\n", paramsH.MULT_INITSPACE);
  printf(
      " paramsH.NUM_BOUNDARY_LAYERS: %d\n paramsH.toleranceZone: %f\n paramsH.NUM_BCE_LAYERS: %d\n "
      "paramsH.solidSurfaceAdjust: %f\n",
      paramsH.NUM_BOUNDARY_LAYERS,
      paramsH.toleranceZone,
      paramsH.NUM_BCE_LAYERS,
      paramsH.solidSurfaceAdjust);
  printf(" paramsH.BASEPRES: %f\n paramsH.LARGE_PRES: %f\n paramsH.deltaPress: %f %f %f\n",
         paramsH.BASEPRES,
         paramsH.LARGE_PRES,
         paramsH.deltaPress.x,
         paramsH.deltaPress.y,
         paramsH.deltaPress.z);
  printf(" paramsH.nPeriod: %d\n paramsH.EPS_XSPH: %f\n paramsH.multViscosity_FSI: %f\n paramsH.rigidRadius: %f\n",
         paramsH.nPeriod,
         paramsH.EPS_XSPH,
         paramsH.multViscosity_FSI,
         paramsH.rigidRadius);
  printf("boxDims: %f, %f, %f\n", paramsH.boxDims.x, paramsH.boxDims.y, paramsH.boxDims.z);
  printf("SIDE: %d, %d, %d\n", paramsH.gridSize.x, paramsH.gridSize.y, paramsH.gridSize.z);
}

// -----------------------------------------------------------------------------
// Specification of the vehicle model
// -----------------------------------------------------------------------------
enum WheelType { CYLINDRICAL, LUGGED };

// Type of wheel/tire (controls both contact and visualization)
WheelType wheel_type = LUGGED;  // CYLINDRICAL;

enum ChassisType { CSPHERE, CBOX, C_SIMPLE_CONVEX_MESH, C_SIMPLE_TRI_MESH, CORIGINAL };

// Type of chassis (controls both contact and visualization)
ChassisType chassis_type = C_SIMPLE_CONVEX_MESH;

// JSON files for vehicle model (using different wheel visualization meshes)
std::string vehicle_file_cyl("hmmwv/vehicle/HMMWV_Vehicle_simple.json");
std::string vehicle_file_lug("hmmwv/vehicle/HMMWV_Vehicle_simple_lugged.json");

// JSON files for powertrain (simple)
std::string simplepowertrain_file("hmmwv/powertrain/HMMWV_SimplePowertrain_Arman.json");

// Initial vehicle position and orientation
// ChVector<> initLoc(-3.0, 0, 0.75);
chrono::ChVector<> initLoc(-9.5, 0, 0.75);
chrono::ChQuaternion<> initRot(1, 0, 0, 0);

// -----------------------------------------------------------------------------
// Simulation parameters MBD ground
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Specification of the terrain
// -----------------------------------------------------------------------------

enum TerrainType { RIGID, GRANULAR };

// Type of terrain
TerrainType terrain_type = RIGID;

// Control visibility of containing bin walls
bool visible_walls = false;

int Id_g = 100;
Real r_g = 0.02;
Real rho_g = 2500;
Real vol_g = (4.0 / 3) * chrono::CH_C_PI * r_g * r_g * r_g;  // Arman: PI or CH_C_PI
Real mass_g = rho_g * vol_g;
chrono::ChVector<> inertia_g = 0.4 * mass_g * r_g * r_g * chrono::ChVector<>(1, 1, 1);

Real mu_g = 0.8;

int num_particles = 1000;

// -----------------------------------------------------------------------------
// Output parameters
// -----------------------------------------------------------------------------

std::ofstream simParams;

chrono::ChTimerParallel fsi_timer;

bool povray_output = true;

const std::string pov_dir = "povFilesHmmwv";

int out_fps = 120;

Real vertical_offset = 0;  // vehicle vertical offset

chrono::utils::VehicleSystem* mVehicle;
chrono::utils::TireContactCallback* tire_cb;
chrono::utils::ChassisContactCallback* chassis_cb;
MyDriverInputs* driver_cb;

#endif  // end of FSI_HMMWV_PARAMS_H_
