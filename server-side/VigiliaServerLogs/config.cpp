class CfgPatches
{
	class VigiliaServerLogs
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class VigiliaServerLogs
	{
		dir="VigiliaServerLogs";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VigiliaServerLogs";
		credits="Reshala/pafnuty";
		author="Eofim";
		authorID="76561199074811285";
		version="2.1.1";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"VigiliaServerLogs/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"VigiliaServerLogs/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"VigiliaServerLogs/scripts/5_Mission"
				};
			};
		};
	};
};
