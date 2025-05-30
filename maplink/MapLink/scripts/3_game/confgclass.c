class MapLinkDepaturePoint extends Managed 
{
	string DisplayName;
	string ServerName;
	int SafeZoneRadius;
	int SafeZoneCoolDown;
	string TerminalType;
	vector Position;
	ref array<ref MapLinkArrivalPointsRef> ArrivalPoints = new array<ref MapLinkArrivalPointsRef>;
	
	void MapLinkDepaturePoint(string displayName)
	{
		DisplayName = displayName;

		if (DisplayName == "Demo000")
		{
			ArrivalPoints.Insert(new MapLinkArrivalPointsRef);
		}
	}
	 
	bool HasArrivalPoint(string arrivalPoint)
	{
		for (int i = 0; i < ArrivalPoints.Count(); i++)
		{
			if(ArrivalPoints.Get(i).ArrivalPointName == arrivalPoint)
			{
				return true;
			}
		}

		return false;
	}
	
	bool GetArrivalPointData(string arrivalPoint, out int id, out int value)
	{
		for (int i = 0; i < ArrivalPoints.Count(); i++)
		{
			if (ArrivalPoints.Get(i).ArrivalPointName == arrivalPoint)
			{
				id = ArrivalPoints.Get(i).AcceptedCurrencyId;
				value = ArrivalPoints.Get(i).Cost;
				return true;
			}
		}

		return false;
	}
}

class MapLinkArrivalPointsRef 
{
	string ArrivalPointName;
	string Icon = "";
	int TransitionWaitTime;
	int Cost;
	int AcceptedCurrencyId;

	MapLinkArrivalPoint Get()
	{
		return GetMapLinkConfig().GetArrivalPoint(ArrivalPointName);
	}
	
	string GetIcon()
	{
		if (Icon.Contains(".paa") || Icon.Contains("set:") || Icon.Contains(".edds"))
		{
			return Icon;
		}

		return "set:maplink_icons image:"+Icon;
	}
}

class MapLinkArrivalPoint extends Managed 
{
	string Name;
	ref array<ref MapLinkSpawnPoint> SpawnPoints = new array<ref MapLinkSpawnPoint>;
	
	void MapLinkArrivalPoint(string displayName)
	{
		Name = displayName;

		if (Name == "Demo000")
		{
			SpawnPoints.Insert(new MapLinkSpawnPoint(true));
		}
	}
	
	int ProtectionTime(string serverName)
	{
		for (int i = 0; i < SpawnPoints.Count(); i++)
		{
			if (SpawnPoints.Get(i).ServerName == serverName)
			{
				return SpawnPoints.Get(i).ProtectionTime;
			}
		}

		MLLog.Err("MapLinkArrivalPoint - Failed to Get Protection Time for " + serverName);
		return -1;
	}
	
	MapLinkSpawnPointPos GetSpawnPos(string serverName)
	{
		for (int i = 0; i < SpawnPoints.Count(); i++)
		{
			if (SpawnPoints.Get(i).ServerName == serverName)
			{
				return SpawnPoints.Get(i).GetSpawnPos();
			}
		}

		MLLog.Err("MapLinkArrivalPoint.GetSpawnPos - Failed to Get SpawnPoint Position from " + Name + " for " + serverName);
		return NULL;
	}
	
	string FallBackServer(string serverName)
	{
		for (int i = 0; i < SpawnPoints.Count(); i++)
		{
			if (SpawnPoints.Get(i).ServerName != serverName)
			{
				return SpawnPoints.Get(i).ServerName;
			}
		}

		return "";
	}
}


class MapLinkSpawnPoint extends Managed
{
	string ServerName;
	string DisplayName;
	int ProtectionTime;
	ref array<ref MapLinkSpawnPointPos> Positions = new array<ref MapLinkSpawnPointPos>;
	
	void MapLinkSpawnPoint(bool defaultCfg = false)
	{
		Positions.Insert(new MapLinkSpawnPointPos());	
	}
	
	MapLinkSpawnPointPos GetSpawnPos()
	{
		if (Positions.Count() == 1)
		{
			return Positions.Get(0);
		}

		int i = 0;
		float Max = 0;
		for (i = 0; i < Positions.Count(); i++)
		{
			Max = Max + Positions.Get(i).GetChance();
		}

		float random = Math.RandomFloatInclusive(0, Max);
		i = -1;
		float chance = 0;
		while (chance < random && ++i < Positions.Count())
		{
			chance = chance + Positions.Get(i).GetChance();
		}
		
		//extra fail safe
		int maxIDX = Positions.Count() - 1;
		if (i > maxIDX)
		{
			i = maxIDX;
		}

		if (i < 0)
		{
			i = 0;
		}
		
		return Positions.Get(i);
	} 
}

class MapLinkSpawnPointPos
{
	float X;
	float Y;
	float Z;
	float D;
	float Radius = -1;
	float Chance = 1;
	
	vector GetPosition()
	{
		float y = Y;
		if (Y <= 0)
		{
			y = GetGame().SurfaceY(X, Z);
		}

		return Vector(X, y, Z);
	}
	
	vector GetOrientation()
	{
		return Vector(D, 0, 0);
	}
	
	vector Get()
	{
		vector pos = GetPosition();
		if (Radius < 2)
		{
			return GetPosition();
		}
		
        vector rndPos = GetRandomPosition();
        int maxCalcs = 25;
        while ((GetGame().SurfaceIsSea(rndPos[0], rndPos[2]) || GetGame().SurfaceIsPond(rndPos[0],rndPos[2])) && maxCalcs > 0) 
		{
			maxCalcs--;
            rndPos = GetRandomPosition();
        }

		if (maxCalcs > 0)
		{
			return rndPos;
		}
		return GetPosition();
	}
	
	vector GetRandomPosition()
    {
		vector pos = Vector(X, 0, Z);
        float rndT = Math.RandomFloat(0,1) * 2 * Math.PI;
        float rndR = Radius * Math.Sqrt(Math.RandomFloat(0,1)) * 0.85;
        //Adding 15% of the radius to make so its not in the center feel like this will just seem less random
        float rndRD = Radius * 0.15;
        rndR = rndR + rndRD;

        float NewX = pos[0] + rndR * Math.Cos(rndT);
        float NewY = pos[2] + rndR * Math.Sin(rndT);

        return Vector(NewX, 0, NewY);
    }
	
	float GetChance()
	{
		if (!Chance || Chance <= 0)
		{
			return 0.01;
		}

		return Chance;
	}

}

class MapLinkCurrency extends Managed 
{
	int ID;
	string Name;
	string Icon;
	bool CanUseRuinedBills = false;
	ref array<ref MapLinkMoneyValue> MoneyValues = new array<ref MapLinkMoneyValue>;
			
	void MapLinkCurrency(int id = 1)
	{
		if (id == -1)
		{
			ID = 0;
			Name = "Rubles";
			Icon = "dollar";
			CanUseRuinedBills = true;
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble100", 100));
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble50", 50));
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble25", 25));
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble10", 10));
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble5", 5));
			MoneyValues.Insert(new MapLinkMoneyValue("MoneyRuble1", 1));
		}

		if (id == -2)
		{
			ID = 1;
			Name = "Tickets";
			Icon = "ticket";
			MoneyValues.Insert(new MapLinkMoneyValue("Chernarus_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("Livonia_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("Namalsk_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("Chiemsee_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("DeerIsle_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("Essker_Ticket", 1));
			MoneyValues.Insert(new MapLinkMoneyValue("Utes_Ticket", 1));
		}
	}
	
	string GetIcon()
	{
		if (Icon.Contains(".paa") || Icon.Contains("set:") || Icon.Contains(".edds"))
		{
			return Icon;
		}

		return "set:maplink_money image:"+Icon;
	}
}

class MapLinkMoneyValue
{
	string Item;
	int Value
	
	void MapLinkMoneyValue(string item, int value)
	{
		Item = item;
		Value = value;
	}
}