#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/UW_TBSelect.h"
#include "Widgets/UW_TBSidebar.h"
#include "UW_TBRoot.generated.h"

/**
 * 
 */


UCLASS()
class DEMO_API UUW_TBRoot : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_TBSelect* Select;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_TBSidebar* Sidebar;
public:

	//function
private:
protected:
public:
	FORCEINLINE UUW_TBSelect* GetSelectWidget() const { return Select; };
	FORCEINLINE UUW_TBSidebar* GetSidebarWidget() const { return Sidebar; };
};
