////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.98
//https://mikero.bytex.digital/Downloads
//'now' is Sat May 17 18:55:49 2025 : 'file' last modified on Sat May 10 14:13:05 2025
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class MapLink
	{
		requiredVersion = 0.1;
		requiredAddons[] = {"UniversalApi","MapLinkBase"};
	};
};
class CfgMods
{
	class MapLink
	{
		dir = "MapLink";
		name = "MapLink";
		credits = "DaemonForge, Iceblade, Dumpgrah";
		author = "DaemonForge";
		authorID = "0";
		version = "0.1";
		extra = 0;
		type = "mod";
		inputs = "MapLink/inputs/inputs.xml";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class imageSets
			{
				files[] = {"MapLink/gui/maplink_icons.imageset","MapLink/gui/maplink_money.imageset"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"MapLink/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"MapLink/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"MapLink/scripts/5_Mission"};
			};
		};
	};
};
