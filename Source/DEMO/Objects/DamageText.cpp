#include "Objects/DamageText.h"
#include "Global.h"
#include "Components/WidgetComponent.h"

ADamageText::ADamageText()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &Widget, "InfoWidget", Scene);

}

void ADamageText::BeginPlay()
{
	Super::BeginPlay();

	// randomize x
	DistanceX = UKismetMathLibrary::RandomFloatInRange(MinX, MaxX);
	if (UKismetMathLibrary::RandomBool())DistanceX *= -1;

	// bind curve or randomize y
	if (bUseTransltaionCurve && YCurve)
	{
		YTimelineFloat.BindUFunction(this, "YSequenceTickFunction");
		YTimeLine.AddInterpFloat(YCurve, YTimelineFloat);
		YTimeLine.PlayFromStart();
	}
	else DistanceY = UKismetMathLibrary::RandomFloatInRange(MinY, MaxY);

	// bind opacity curve
	if (bUseOpacityCurve && OpacityCurve) 
	{
		OpacityTimelineFloat.BindUFunction(this, "OpacitySequenceTickFunction");
		OpacityTimeLine.AddInterpFloat(OpacityCurve, OpacityTimelineFloat);
		OpacityTimeLine.PlayFromStart();
	}
}

void ADamageText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float since = GetGameTimeSinceCreation();

	// set translation
	CurrentX = since * DistanceX;
	if (bUseTransltaionCurve && YCurve)YTimeLine.TickTimeline(DeltaTime);
	else CurrentY = UKismetMathLibrary::Sin((since / InitialLifeSpan) * PI * RateY) * DistanceY;
	Widget->GetUserWidgetObject()->SetRenderTranslation(FVector2D(CurrentX, CurrentY));

	// set opacity
	if (bUseOpacityCurve && OpacityCurve)OpacityTimeLine.TickTimeline(DeltaTime);
	else CurrentOpacity = MinOpacity + (1 - MinOpacity) * (InitialLifeSpan - since) / InitialLifeSpan;
	Widget->GetUserWidgetObject()->SetRenderOpacity(CurrentOpacity);
}

void ADamageText::YSequenceTickFunction(float Value)
{
	CurrentY = Value;
}

void ADamageText::OpacitySequenceTickFunction(float Value)
{
	CurrentOpacity = Value;
}

void ADamageText::Init(float InDamage, bool IsCritical)
{
	Damage = InDamage;
	Critical = IsCritical;
}