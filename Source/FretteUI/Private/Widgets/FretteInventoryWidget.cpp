#include "Widgets/FretteInventoryWidget.h"

#include "MVVMSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "View/MVVMView.h"
#include "Widgets/FretteInventorySlotWidget.h"

// how many slots there are is dictated by the designer-placed slot widgets
// so we must build the slot viewmodels array from the inventory widget
// TODO: There might be a way to generate the slots from a count in the inventory viewmodel,
// then request them to be bound by the designer in the widget. Like BindWidget uproperty but dynamic?
void UFretteInventoryWidget::InitializeSlotViewmodels() const
{
	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);
	
	for (UWidget* Widget : Widgets)
	{
		const auto* SlotWidget = Cast<UFretteInventorySlotWidget>(Widget);
		if (SlotWidget == nullptr)
			continue;
		
		const UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(SlotWidget);
		check(View);
		
		static const FName NAME_SlotVM = TEXT("SlotVM");
		auto* SlotVM = Cast<UFretteSlotsInventorySlotVM>(View->GetViewModel(NAME_SlotVM).GetObject());
		check(SlotVM)
		
		SlotVM->ParentVM = SlotsVM;
		SlotVM->SetCompatibleTag(SlotWidget->CompatibleSlotType);
		SlotsVM->Slots.Add(SlotVM);
	}
}

void UFretteInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(SlotsVM);
	
	InitializeSlotViewmodels();
}