#pragma once

#include "Unreal.hpp"
#include "Globals.hpp"
#include "Structs.hpp"
#include "Offsets.hpp"
#include "enums.hpp"

namespace Functions
{
	static void InitConsole() {
		SpawnObjectParams params;

		params.ObjectClass = FindObject("Class /Script/FortniteGame.FortConsole");
		params.Outer = Globals::GameViewport;

		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);

		UObject** ViewportConsole = reinterpret_cast<UObject**>(__int64(Globals::GameViewport) + Offsets::GameViewportClient::ViewportConsole);
		*ViewportConsole = params.ReturnValue;
	}

	static void InitCheatManager() {
		/*SpawnObjectParams params;

		params.ObjectClass = FindObject("Class /Script/Engine.CheatManager");
		params.Outer = Globals::PlayerController;

		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);

		UObject** CheatManager = reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::PlayerController::CheatManager);
		*CheatManager = params.ReturnValue;*/
		Globals::CheatMananger = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::PlayerController::CheatManager);
	}

	static inline void SwitchLevel(FString URL)
	{
		static auto fn = FindObject("Function /Script/Engine.PlayerController.SwitchLevel");
		ProcessEvent(Globals::PlayerController, fn, &URL);
	}

	static void Possess(UObject* Inpawn) {
		auto PossessFunc = FindObject("Function /Script/Engine.Controller.Possess");
		struct
		{
			class UObject* InPawn;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		} possesparams;
		possesparams.InPawn = Inpawn;
		ProcessEvent(Globals::PlayerController, PossessFunc, &possesparams);
	}

	static inline UObject* SpawnActor(UObject* ActorClass, FVector loc) {
		auto spawnfunction1 = FindObject("Function /Script/Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
		auto spawnfunction2 = FindObject("Function /Script/Engine.GameplayStatics.FinishSpawningActor");

		UGameplayStatics_BeginDeferredActorSpawnFromClass_Params bdasfc;
		UGameplayStatics_FinishSpawningActor_Params fsap;

		bdasfc.ActorClass = (UClass*)ActorClass;
		bdasfc.CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		bdasfc.SpawnTransform.Rotation = FQuat{ 0,0,0,0 };
		bdasfc.SpawnTransform.Scale3D = FVector{ 1,1,1 };
		bdasfc.SpawnTransform.Translation = loc;
		bdasfc.WorldContextObject = Globals::World;
		bdasfc.Owner = nullptr;

		auto GameplayStatics = FindObject("Default__GameplayStatics");

		std::cout << GameplayStatics->GetFullName();
		std::cout << spawnfunction1->GetFullName();

		ProcessEvent(GameplayStatics, spawnfunction1, &bdasfc);

		fsap.Actor = (UObject*)bdasfc.ReturnValue;
		fsap.SpawnTransform = bdasfc.SpawnTransform;

		ProcessEvent(GameplayStatics, spawnfunction2, &fsap);

		return (UObject*)fsap.ReturnValue;
	}

	static inline void DestroyAll(const char* ClassToDestroy)
	{
		UObject* LocatedClass = FindObject(ClassToDestroy);
		ProcessEvent(Globals::CheatMananger, FindObject("Function /Script/Engine.CheatManager.DestroyAll"), &LocatedClass);
	}

	static inline void StartMatch()
	{
		static auto fn = FindObject("Function /Script/Engine.GameMode.StartMatch");
		ProcessEvent(Globals::GameMode, fn, nullptr);
	}

	static inline void ServerReadyToStartMatch()
	{
		static auto fn = FindObject("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
		ProcessEvent(Globals::PlayerController, fn, nullptr);
	}

	static inline void SetMaxHealth()
	{
		auto PlayerState = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::Pawn::PlayerState);
		auto CurrentHealth = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::CurrentHealth);
		auto MaxHealth = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::MaxHealth);
		auto CurrentSheild = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::CurrentSheild);
		auto MaxSheild = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::MaxSheild);
		*CurrentHealth = 100;
		*MaxHealth = 100;
		*CurrentSheild = 100;
		*MaxSheild = 100;
	}

	static inline void SetupCharacterParts()
	{
		UObject* DefaultHead = FindObject("CustomCharacterPart /Game/Athena/Heroes/Meshes/Heads/Dev_TestAsset_Head_M_XL.Dev_TestAsset_Head_M_XL");
		UObject* DefaultBody = FindObject("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/Dev_TestAsset_Body_M_XL.Dev_TestAsset_Body_M_XL");
		UObject* FortHero = FindObject("FortHero /Engine/Transient.FortHero_");
		UObject* PlayerState = *reinterpret_cast<UObject**>(__int64(Globals::Pawn) + Offsets::Pawn::PlayerState);

		auto CharacterParts = reinterpret_cast<TArray<UObject*>*>(__int64(FortHero) + Offsets::FortHero::CharacterParts);

		CharacterParts->operator[](0) = DefaultBody;
		CharacterParts->operator[](1) = DefaultHead;

		struct Params
		{
			UObject* WorldContextObject;
			TArray<UObject*> CharacterParts;
			UObject* PlayerState;
			bool bSuccess;
		};
		Params params;
		params.WorldContextObject = Globals::World;
		params.CharacterParts = *CharacterParts;
		params.PlayerState = PlayerState;

		static auto KismetLib = FindObject("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary");
		static auto fn = FindObject("Function /Script/FortniteGame.FortKismetLibrary.ApplyCharacterCosmetics");

		ProcessEvent(KismetLib, fn, &params);
	}

	static inline void AddMovementInput(UObject* Pawn, FVector Loc, float ScaleValue, bool bForce)
	{
		struct Params
		{
			struct FVector WorldDirection;
			float ScaleValue;
			bool bForce;
		};
		Params params;
		params.WorldDirection = Loc;
		params.ScaleValue = ScaleValue;
		params.bForce = bForce;

		static auto fn = FindObject("Function /Script/Engine.Pawn.AddMovementInput");
		ProcessEvent(Pawn, fn, &params);
	}

	static inline FVector GetActorRightVector(UObject* Actor)
	{
		struct Params
		{
			struct FVector ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Actor.GetActorRightVector");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline FVector GetActorForwardVector(UObject* Actor)
	{
		struct Params
		{
			struct FVector ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Actor.GetActorForwardVector");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline bool K2_SetActorLocation(UObject* Actor, FVector Location)
	{
		struct Params
		{
			struct FVector NewLocation;
			bool bSweep;
			struct FHitResult SweepHitResult;
			bool bTeleport;
			bool ReturnValue;
		};
		Params params;
		params.NewLocation = Location;
		params.bSweep = true;
		params.SweepHitResult = FHitResult();
		params.bTeleport = true;

		static auto fn = FindObject("Function /Script/Engine.Actor.K2_SetActorLocation");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline bool bIsJumpProvidingForce(UObject* Pawn)
	{
		struct Params
		{
			bool ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Character.IsJumpProvidingForce");
		ProcessEvent(Pawn, fn, &params);

		return params.ReturnValue;
	}

	static inline void Jump(UObject* Pawn)
	{
		static auto fn = FindObject("Function /Script/Engine.Character.Jump");
		ProcessEvent(Pawn, fn, nullptr);
	}
}