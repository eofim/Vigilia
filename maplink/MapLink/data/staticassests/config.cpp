////////////////////////////////////////////////////////////////////
//DeRap: data\StaticAssests\config.bin
//Produced from mikero's Dos Tools Dll version 9.98
//https://mikero.bytex.digital/Downloads
//'now' is Sat May 17 18:55:49 2025 : 'file' last modified on Sat May 10 14:13:05 2025
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class Hive_Statics
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class GN_Hive_Boat: HouseNoDestruct
	{
		scope = 2;
		displayName = "GN_Hive_Boat";
		descriptionShort = "Testing";
		model = "MapLink\data\StaticAssests\Boat.p3d";
	};
	class Hive_Small_Boarding_Plank: HouseNoDestruct
	{
		scope = 2;
		displayName = "GN_Hive_Boat";
		descriptionShort = "Testing";
		model = "MapLink\data\StaticAssests\ShortPlank.p3d";
		physLayer = "item_large";
		itemBehaviour = 2;
	};
	class Hive_Medium_Boarding_Plank: HouseNoDestruct
	{
		scope = 2;
		displayName = "GN_Hive_Boat";
		descriptionShort = "Testing";
		model = "MapLink\data\StaticAssests\MediumPLank.p3d";
		physLayer = "item_large";
		itemBehaviour = 2;
	};
	class Hive_Long_Boarding_Plank: HouseNoDestruct
	{
		scope = 2;
		displayName = "GN_Hive_Boat";
		descriptionShort = "Testing";
		model = "MapLink\data\StaticAssests\LongPlank.p3d";
		physLayer = "item_large";
		itemBehaviour = 2;
	};
	class HiveStaticHeli: HouseNoDestruct
	{
		scope = 2;
		displayName = "GN_Hive_Boat";
		descriptionShort = "";
		model = "MapLink\data\StaticAssests\StaticHeli.p3d";
		forceFarBubble = "false";
		carveNavmesh = 1;
		rotationFlags = 2;
		itemSize[] = {1,1};
		weight = 1000000;
	};
};
