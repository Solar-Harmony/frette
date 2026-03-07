// #pragma once
//
// #include "CoreMinimal.h"
// #include "AbilitySystemComponent.h"
// #include "Damage/FretteBodyPartAttributeSet.h"
// #include "Net/UnrealNetwork.h"
// #include "FretteBodyPartDefinitions.generated.h"
//
// // Shared boilerplate so each derived class has its *own* reflected Health/MaxHealth attributes.
//
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_Head : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
//
// 	public:
// 		UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
// 		FGameplayAttributeData Health;
// 		ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Head, Health);
//
// 		UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly)
// 		FGameplayAttributeData MaxHealth;
// 		ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Head, MaxHealth);
//
// 		virtual FGameplayAttribute GetHealthGameplayAttribute() const override
// 		{
// 			return GetHealthAttribute();
// 		}
//
// 		virtual FGameplayAttribute GetMaxHealthGameplayAttribute() const override
// 		{
// 			return GetMaxHealthAttribute();
// 		}
//
// 		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
// 		virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
//
// 		UFUNCTION()
// 		void OnRep_Health(const FGameplayAttributeData& OldHealth);
//
// 		UFUNCTION()
// 		void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
// };
//
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_Torso : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
// 	FGameplayAttributeData Health;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Torso, Health);
//
// 	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly)
// 	FGameplayAttributeData MaxHealth;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Torso, MaxHealth);
//
// 	virtual FGameplayAttribute GetHealthGameplayAttribute() const override
// 	{
// 		return GetHealthAttribute();
// 	}
//
// 	virtual FGameplayAttribute GetMaxHealthGameplayAttribute() const override
// 	{
// 		return GetMaxHealthAttribute();
// 	}
//
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
// 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
//
// 	UFUNCTION()
// 	void OnRep_Health(const FGameplayAttributeData& OldHealth);
//
// 	UFUNCTION()
// 	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_LeftArm : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
// 	FGameplayAttributeData Health;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Head, Health);
//
// 	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly)
// 	FGameplayAttributeData MaxHealth;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_Head, MaxHealth);
//
// 	virtual FGameplayAttribute GetHealthGameplayAttribute() const override
// 	{
// 		return GetHealthAttribute();
// 	}
//
// 	virtual FGameplayAttribute GetMaxHealthGameplayAttribute() const override
// 	{
// 		return GetMaxHealthAttribute();
// 	}
//
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
// 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
//
// 	UFUNCTION()
// 	void OnRep_Health(const FGameplayAttributeData& OldHealth);
//
// 	UFUNCTION()
// 	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_RightArm : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
// 	FGameplayAttributeData Health;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_RightArm, Health);
//
// 	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly)
// 	FGameplayAttributeData MaxHealth;
// 	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet_RightArm, MaxHealth);
//
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
// 	{
// 		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 		DOREPLIFETIME_CONDITION_NOTIFY(UFretteBodyPartAttributeSet_RightArm, Health, COND_None, REPNOTIFY_Always);
// 		DOREPLIFETIME_CONDITION_NOTIFY(UFretteBodyPartAttributeSet_RightArm, MaxHealth, COND_None, REPNOTIFY_Always);
// 	}
//
// 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override
// 	{
// 		Super::PreAttributeBaseChange(Attribute, NewValue);
//
// 		if (Attribute == GetHealthAttribute())
// 		{
// 			NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
// 		}
// 	}
//
// 	UFUNCTION()
// 	void OnRep_Health(const FGameplayAttributeData& OldHealth) const
// 	{
// 		GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteBodyPartAttributeSet_RightArm, Health, OldHealth);
// 	}
//
// 	UFUNCTION()
// 	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
// 	{
// 		GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteBodyPartAttributeSet_RightArm, MaxHealth, OldMaxHealth);
// 	}
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_LeftHand : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_RightHand : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_LeftLeg : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_RightLeg : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_LeftFoot : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };
//
// UCLASS()
// class FRETTE_API UFretteBodyPartAttributeSet_RightFoot : public UFretteBodyPartAttributeSet
// {
// 	GENERATED_BODY()
// };