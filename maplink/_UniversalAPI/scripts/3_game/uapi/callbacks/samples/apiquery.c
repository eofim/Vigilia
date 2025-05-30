class ApiQueryBase : RestCallback
{
	int ResultLimit;
	string Mod;
	string Query;

	/*
	Replace with your Results
	ref array<ref UApiConfigBase> Results;
	*/
	int Count;

	string ToJson()
	{
		// Override and replace with your class name
		string jsonString = JsonFileLoader<ApiQueryBase>.JsonMakeData(this);
		Print("[UAPI] Error You didn't override ToJson: " + jsonString);
		return jsonString;
	}

	override void OnSuccess(string data, int dataSize)
	{
		// Change to your class name
		JsonFileLoader<ApiQueryBase>.JsonLoadData(data, this);

		if (this)
		{

		}
		else
		{
			Print("[UAPI] CallBack Failed errorCode: Invalid Data");
		}
	}
}