// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"

#include <list>
#include <unordered_map>

#include "KuTypes.generated.h"

class AChunkBase;
class AFieldItem;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UKuTypes : public UObject
{
	GENERATED_BODY()
};

// using FIdxVector = FIntVector;
/** 3차원 정수형 인덱스를 보관하는 Vector 입니다. */
using FIdxVector = UE::Math::TIntVector3<int16>;

using FVoxel3DArray = std::vector<std::vector<std::vector<float>>>;

/** 콜리전 프로파일 이름 모음 */
struct FKuCollisionProfile
{
	/** 필드에 스폰된 아이템. */
	static const FName FieldItem;

	/** 필드에 스폰하기 전의 미리보기 아이템. */
	static const FName PreviewFieldItem;

	/** 커서, 클릭, 터치 등 라인트레이스 감지 전용. */
	static const FName LineTraceOnly;

	/** UI 오브젝트. UI 감지 전용 콜리전. 선택, 이동 등에 사용하는 UITrace에만 반응하며 다른 트레이스 채널은 무시함. */
	static const FName UITraceOnly;
};

struct FKuCollisionChannel
{
	/** 오버랩 전용 트레이스 채널 */
	static const ECollisionChannel FieldItemTrace;
};


/** 스텐실 외곽선 색상*/
UENUM()
enum class EStencilOutline : uint8
{
	None = 0,
	Preview_Buildable,
	Preview_Unbuildable,
	Friendly_Hover,
	Friendly_Select,
	Enemy_Hover,
	Enemy_Select,
};

// 이는 게임에 하드코딩되어 있으며 Steam에 업로드된 아이템 정의 ID와 일치합니다.
enum ESpaceWarItemDefIDs
{
	k_SpaceWarItem_TimedDropList = 10,
	k_SpaceWarItem_ShipDecoration1 = 100,
	k_SpaceWarItem_ShipDecoration2 = 101,
	k_SpaceWarItem_ShipDecoration3 = 102,
	k_SpaceWarItem_ShipDecoration4 = 103,
	k_SpaceWarItem_ShipWeapon1 = 110,
	k_SpaceWarItem_ShipWeapon2 = 111,
	k_SpaceWarItem_ShipSpecial1 = 120,
	k_SpaceWarItem_ShipSpecial2 = 121
};


//~============================================
// 아이템 관련

/** 아이템 종류별 ID */
using TItemDefID = int32;

UENUM()
enum class EItemTag : uint8
{
	None,
	Furniture,
	Architecture,


	Size
};

USTRUCT()
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemTableRow();

	/**
	 * 아이템 종류에 따른 ID입니다.
	 * 각 아이템 ID는 서로 고유하고, 양수입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemDefID;

	/**
	 * 아이템의 시그니처 이름입니다.
	 * 아이템과 관련된 에셋 이름을 식별하는데 사용합니다.
	 * */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemSignatureName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AFieldItem> ItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories="Item"))
	FGameplayTagContainer ItemTags;
};

USTRUCT()
struct FItemData
{
	GENERATED_BODY()

public:
	FItemData() = default;

	TItemDefID ItemDefID;
	uint32 Count;
};

/** 인벤토리 저장용 데이터. 인벤토리의 위치와 아이템 종류를 저장합니다. */
USTRUCT()
struct FItemInvenSaveData
{
	GENERATED_BODY()
	FItemInvenSaveData();

	FItemInvenSaveData(const uint32 InCol, const uint32 InRow, const FItemData& InItems);

	UPROPERTY()
	uint32 Col;
	
	UPROPERTY()
	uint32 Row;
	
	UPROPERTY()
	FItemData Items;
};


/** 필드 저장용 데이터. 아이템 종류, 위치, 부착 위치 등을 저장합니다. */
USTRUCT()
struct FItemFieldSaveData
{
	GENERATED_BODY()
	FItemFieldSaveData();
	
	FItemFieldSaveData(const FVector3d& InLocation, const FRotator3d& InRotator, const FItemData& InItems, const TArray<FItemFieldSaveData>& InAttachedItems);

	UPROPERTY()
	FVector3d Location;
	
	UPROPERTY()
	FRotator3d Rotator;
	
	UPROPERTY()
	FItemData Items;

	TArray<FItemFieldSaveData> AttachedItems;
};


using TQueryID = int64;
enum { INVALID_QUERY_ID = -1 };

/**
 * Non-GameThread에서 사용할 비동기 메시 데이터 계산용 데이터를 묶은 구조체입니다.
 * 람다 캡쳐 및 함수 매개변수로 사용됩니다.
 */
struct FAsyncChunkMeshData
{
	FAsyncChunkMeshData() = default;
	explicit FAsyncChunkMeshData(const AChunkBase* InChunkBase);

	FAsyncChunkMeshData(const FAsyncChunkMeshData& Other) = delete;
	FAsyncChunkMeshData& operator=(const FAsyncChunkMeshData& Other) = delete;

	FAsyncChunkMeshData(FAsyncChunkMeshData&& Other) noexcept;
	FAsyncChunkMeshData& operator=(FAsyncChunkMeshData&& Other) noexcept;

	TQueryID InQueryID;
	FIntVector InChunkIdx;
	FIdxVector InChunkSizeVec;
	FVoxel3DArray InChunkVoxels;
	float InUnitVoxelLength;
	bool InbUseSmoothingNormal;
	float InSurfaceLevel;
};

/** std::hash<>가 지원하는 타입인지 확인 */
template <typename T>
concept is_exists_std_hasher = requires(T t)
{
	{ std::hash<T>{}(t) } -> std::convertible_to<size_t>;
};

/** std::hash가 지원하지 않는 타입은 UE가 지원하는 해시 함수를 사용합니다. */
template <typename T>
struct ue_hasher
{
	size_t operator()(const T& InVal) const
	{
		if constexpr (is_exists_std_hasher<T>)
		{
			return std::hash<T>{}(InVal);
		}
		else
		{
			return GetTypeHash(InVal);
		}
	}
};

/**
 * 원소가 추가된 순서를 유지하는 해시 맵입니다. 
 * value의 선입선출을 유지하고, key를 통해 삭제할 수 있습니다.
 * 탐색 및 삽입, 삭제 모두 상수시간 복잡도를 가집니다.
 */
template <typename _Kty, typename _Valty, typename _Hasher = ue_hasher<_Kty>, typename _Keyeq = std::equal_to<_Kty>>
class hash_queue
{
	using pair_list = std::list<std::pair<_Kty, _Valty>>;

	pair_list keyValQueue; // key, value queue
	
	std::unordered_map<_Kty, typename pair_list::iterator, _Hasher, _Keyeq> keyListHash; // key, list_iterator 매핑 테이블

public:
	void push(const _Kty& key, const _Valty& value)
	{
		// 순서 유지를 위해, 이미 존재할 경우 삭제 후 다시 삽입
		if (keyListHash.contains(key))
		{
			erase(key);
		}

		// 리스트에 삽입 및 해시 테이블에 위치 저장
		keyListHash[key] = keyValQueue.emplace(keyValQueue.end(), key, value);
	}

	_Valty top()
	{
		if (keyValQueue.empty())
		{
			return {};
		}

		return keyValQueue.front().second;
	}

	void pop()
	{
		if (keyValQueue.empty())
		{
			return;
		}

		keyListHash.erase(keyValQueue.front().first);
		keyValQueue.pop_front();
	}

	bool erase(const _Kty& key)
	{
		const auto findIt = keyListHash.find(key);
		if (findIt == keyListHash.end())
		{
			return false;
		}

		keyValQueue.erase(findIt->second);
		keyListHash.erase(findIt);
		return true;
	}

	_Valty* find(const _Kty& key)
	{
		const auto findIt = keyListHash.find(key);
		if (findIt == keyListHash.end())
		{
			return nullptr;
		}

		return &(findIt->second->second);
	}

	bool empty()
	{
		return keyValQueue.empty();
	}
};

UENUM(BlueprintType)
enum class EChunkMeshGenerateType : uint8
{
	None

  , UsedCreateMeshSection // Proc메시 + CreateMeshSection 사용	
  , UsedSetProcMove       // Proc메시 + Move 편법 사용
  , UsedAsyncProcMesh     // AsyncProc메시 사용
  , UsedDynamic           // 다이나믹 메시 사용


  , Size UMETA(Hidden)
};
