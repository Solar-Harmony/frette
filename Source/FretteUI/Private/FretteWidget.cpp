#include "FretteWidget.h"

#include "FretteViewModel.h"
#include "View/MVVMView.h"

UFretteWidget* UFretteWidget::CreateFretteWidget(
	APlayerController* OwningPlayer, 
	TSubclassOf<UFretteWidget> WidgetClass, 
	TSubclassOf<UFretteViewModel> ViewModelClass
) {
	UFretteWidget* Widget = CreateWidget<UFretteWidget>(OwningPlayer, WidgetClass);
	if (!ensureAlways(Widget))
		return nullptr;
	
	Widget->ViewModel = NewObject<UFretteViewModel>(Widget, ViewModelClass);
	if (!ensureAlways(Widget->ViewModel))
		return nullptr;
	
	Widget->ViewModel->Bind(OwningPlayer);
	UMVVMView* MVVMExtension = Widget->GetExtension<UMVVMView>();
	if (!ensureAlways(MVVMExtension))
		return nullptr;
	
	MVVMExtension->SetViewModelByClass(Widget->ViewModel);
	Widget->AddToViewport();
	return Widget;
}
