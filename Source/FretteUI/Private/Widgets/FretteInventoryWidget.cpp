#include "Widgets/FretteInventoryWidget.h"

#include "MVVMSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Frette/Frette.h"
#include "View/MVVMView.h"
#include "Widgets/FretteInventorySlotWidget.h"

void UFretteInventoryWidget::NativeConstruct()
{
	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);
	
	int32 SlotIdx = 0;
	
	check(SlotsVM);
	
	for (UWidget* Widget : Widgets)
	{
		UFretteInventorySlotWidget* SlotWidget = Cast<UFretteInventorySlotWidget>(Widget);
		if (SlotWidget == nullptr)
			continue;
		
		const UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(SlotWidget);
		check(View);
		
		static const FName NAME_SlotVM = TEXT("SlotVM");
		auto* SlotVM = Cast<UFretteSlotsInventorySlotVM>(View->GetViewModel(NAME_SlotVM).GetObject());
		check(SlotVM)
		
		SlotWidget->SlotID = SlotIdx++;
		SlotVM->ParentVM = SlotsVM;
		SlotsVM->SlotIDToItemMap.Add(SlotWidget->SlotID, SlotVM);
	}
}