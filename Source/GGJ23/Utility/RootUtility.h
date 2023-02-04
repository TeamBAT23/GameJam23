#pragma once
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"

#include "RootUtility.generated.h"

UCLASS()
class URootUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Get a curve value within a thread safe function.
	 * Note: This isn't technically thread-safe since the asset can be modified.
	 * Only use this for constant curve assets and never for dynamically changing assets.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable,  meta = (BlueprintThreadSafe))
	static float GetFloatValue_ThreadSafe(UCurveFloat* FloatCurve, float InTime);

	/**
	 * Get a curve value within a thread safe function.
	 * Note: This isn't technically thread-safe since the asset can be modified.
	 * Only use this for constant curve assets and never for dynamically changing assets.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable,  meta = (BlueprintThreadSafe))
	static FVector GetVectorValue_ThreadSafe(UCurveVector* VectorCurve, float InTime);

	/**
	 * Get a curve value within a thread safe function.
	 * Note: This isn't technically thread-safe since the asset can be modified.
	 * Only use this for constant curve assets and never for dynamically changing assets.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable,  meta = (BlueprintThreadSafe))
	static FLinearColor GetLinearColorValue_ThreadSafe(UCurveLinearColor* ColorCurve, float InTime);

	/**
	 * @return true if the game is running
	 */
	UFUNCTION(BlueprintCallable)
	static bool IsGameRunning(UObject* ContextObject);
};
