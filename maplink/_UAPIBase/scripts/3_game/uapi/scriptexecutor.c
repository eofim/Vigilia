// From Wardog and DaOne

class UScriptExec
{
	static ref array<ScriptModule> m_ScriptModules = new array<ScriptModule>;

	// fire and forget, method style execution
	static void Func(string code, bool deleteFile = true)
	{
		FileHandle file;
		MakeDirectory("$profile:UApi/Scripts/");
		string filename = string.Format("script_%1", GetDateTime());
		string script_path = string.Format("$profile:UApi/Scripts/%1.c", filename);
		int i = 0;

		while (FileExist(script_path) && i < 32)
		{
			filename = string.Format("script_%1_%2_%3", GetDateTime(), i++, Math.RandomInt(10000, 99999));
			script_path = string.Format("$profile:UApi/Scripts/%1.c", filename);
		}

		file = OpenFile(script_path, FileMode.WRITE);
		if (file)
		{
			string ex_script = "static void main()\n{\n" + code + "\n}";
			FPrintln(file, ex_script);
			CloseFile(file);
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, script_path, true);

		if (!module)
		{
			Error2("[UAPI][FUNC]", "Invalid syntax! Review your script. " + code);
			return;
		}

		module.Call(null, "main", null);

		if (deleteFile)
		{
			DeleteFile(script_path);
		}
	}

	static ScriptModule Run(string path)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "Main", null, null);
		return module;
	}

	static ScriptModule Run(string path, out int result)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "Main", result, null);
		return module;
	}

	static ScriptModule Run(string path, out bool result)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "Main", result, null);
		return module;
	}

	static ScriptModule Run(string path, out float result)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "Main", result, null);
		return module;
	}

	static ScriptModule Run(string path, out string result)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "Main", result, null);
		return module;
	}

	static ScriptModule Load(string path)
	{
		return LoadInMission(path);
	}

	static ScriptModule LoadInGame(string path)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GameScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		return module;
	}

	static ScriptModule LoadInMission(string path)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		return module;
	}

	static Class Spawn(string className)
	{
		if (className.ToType())
		{
			return className.ToType().Spawn();
		}
		else
		{
			Error2("[UAPI][SPAWN]", "'" + className + "' wasn't a valid classname couldn't spawn");
		}

		return NULL;
	}

	static bool SpawnScriptBase(string className, out UApiScriptBase instance)
	{
		if (className.ToType())
		{
			instance = UApiScriptBase.Cast(className.ToType().Spawn());
			instance.Init();
			return true;
		}

		return false;
	}

	private static string GetDateTime()
	{
		int year, month, day;
		int hour, minute, second;

		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		return year.ToStringLen(4) + "-" + month.ToStringLen(2) + "-" + day.ToStringLen(2) + "_" + hour.ToStringLen(2) + minute.ToStringLen(2) + second.ToStringLen(2);
	}

	static ScriptModule ExecMission(string path, out Class instance)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "GetInstance", instance, null);
		module.CallFunction(instance, "Init", null, null);
		return module;
	}

	static ScriptModule ExecGame(string path, out Class instance)
	{
		if (!FileExist(path))
		{
			Error2("[UAPI][EXEC]", "Unable to find file at path " + path);
			return null;
		}

		ScriptModule module = ScriptModule.LoadScript(GetGame().GameScript, path, true);
		if (!module)
		{
			Error2("[UAPI][EXEC]", "Invalid syntax! Review your script. " + path);
			return null;
		}

		m_ScriptModules.Insert(module);
		module.CallFunction(null, "GetInstance", instance, null);
		module.CallFunction(instance, "Init", null, null);
		return module;
	}
}
