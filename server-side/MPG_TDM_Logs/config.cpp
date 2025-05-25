class CfgPatches
{
	class MPG_TDM_Logs
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
	class MPG_TDM_Logs
	{
		dir="MPG_TDM_Logs";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="MPG_TDM_Logs";
		credits="Reshala";
		author="pafnuty";
		authorID="76561199074811285";
		version="2.1";
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
					"MPG_TDM_Logs/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"MPG_TDM_Logs/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"MPG_TDM_Logs/scripts/5_Mission"
				};
			};
		};
	};
};
