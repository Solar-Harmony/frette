#include "FretteWidget.h"

#include "FretteViewModel.h"
#include "View/MVVMView.h"

UFretteWidget* UFretteWidget::CreateFretteWidget(
	APlayerController* OwningPlayer, 
	TSubclassOf<UFretteWidget> WidgetClass, 
	TSubclassOf<UFretteViewModel> ViewModelClass
) {
	// TODO: ensure setup to Manual binding mode
	UFretteWidget* Widget = CreateWidget<UFretteWidget>(OwningPlayer, WidgetClass);
	Widget->ViewModel = NewObject<UFretteViewModel>(Widget, ViewModelClass);
	Widget->ViewModel->Bind(OwningPlayer);
	Widget->GetExtension<UMVVMView>()->SetViewModelByClass(Widget->ViewModel);
	Widget->AddToViewport();
	
	return Widget;
}
