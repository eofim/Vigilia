class UApiCryptoRequest extends UApiObject_Base 
{
	ref TStringArray From = new TStringArray;

	void UApiCryptoRequest(TStringArray from) 
	{
		From.Copy(from);
	}

	override string ToJson() 
	{
		string jsonString = UApiJSONHandler<UApiCryptoRequest>.ToString(this);
		return jsonString;
	}
}

class UApiCryptoConvertRequest extends UApiObject_Base 
{
	float Value = 0;

	void UApiCryptoConvertRequest(float value) 
	{
		Value = value;
	}

	override string ToJson() 
	{
		string jsonString = UApiJSONHandler<UApiCryptoConvertRequest>.ToString(this);
		return jsonString;
	}
}

class UApiCryptoConvertResult extends StatusObject 
{
	float Value;

	float Get() 
	{
		return Value;
	}
} 

class UApiCryptoResults extends StatusObject 
{
	ref map<string, float> Values;

	map<string, float> Get() 
	{
		ref map<string, float> rValue = new map<string, float>;
		rValue.Copy(Values);
		return rValue;
	}
}