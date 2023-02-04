#include "RootUtility.h"

float URootUtility::GetFloatValue_ThreadSafe(UCurveFloat* FloatCurve, float InTime)
{
	if(FloatCurve)
	{
		return FloatCurve->GetFloatValue(InTime);
	}
	return 0.0f;
}

FVector URootUtility::GetVectorValue_ThreadSafe(UCurveVector* VectorCurve, float InTime)
{
	if(VectorCurve)
	{
		return VectorCurve->GetVectorValue(InTime);
	}
	return FVector::ZeroVector;
}

FLinearColor URootUtility::GetLinearColorValue_ThreadSafe(UCurveLinearColor* ColorCurve, float InTime)
{
	if(ColorCurve)
	{
		return ColorCurve->GetLinearColorValue(InTime);
	}
	return FLinearColor::Black;
}

bool URootUtility::IsGameRunning(UObject* ContextObject)
{
	if(ContextObject)
	{
		EWorldType::Type Type = ContextObject->GetWorld()->WorldType.GetValue();
		return Type == EWorldType::PIE || Type == EWorldType::Game;
	}
	return false;
}
