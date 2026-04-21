#include "FretteClueTemplateCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Framework/Text/BaseTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/TextLayout.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Styling/AppStyle.h"
#include "Editor.h"

class FClueTextLayoutMarshaller : public FBaseTextLayoutMarshaller
{
public:
	static TSharedRef<FClueTextLayoutMarshaller> Create()
	{
		return MakeShareable(new FClueTextLayoutMarshaller());
	}

	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override
	{
		TargetTextLayout.ClearLines();

		FTextBlockStyle NormalStyle = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
		FSlateFontInfo DefaultFont = NormalStyle.Font;
		DefaultFont.Size = 16;
		NormalStyle.SetFont(DefaultFont);

		FTextBlockStyle ValidStyle = NormalStyle;
		FSlateFontInfo BoldFont = FAppStyle::Get().GetFontStyle("BoldFont");
		BoldFont.Size = 16;
		ValidStyle.SetFont(BoldFont);

		FTextBlockStyle InvalidStyle = NormalStyle;
		InvalidStyle.SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		InvalidStyle.SetFont(BoldFont);

		TArray<FTextLayout::FNewLineData> LinesToAdd;
		int32 CurrentIdx = 0;
		while (CurrentIdx < SourceString.Len() || (CurrentIdx == SourceString.Len() && SourceString.EndsWith(TEXT("\n"))))
		{
			int32 LineEndIdx = SourceString.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, CurrentIdx);
			if (LineEndIdx == INDEX_NONE)
			{
				LineEndIdx = SourceString.Len();
			}
			
			int32 TrueLineEnd = LineEndIdx;
			if (TrueLineEnd > CurrentIdx && SourceString[TrueLineEnd - 1] == '\r')
			{
				TrueLineEnd--;
			}

			FString LineString = SourceString.Mid(CurrentIdx, TrueLineEnd - CurrentIdx);
			TSharedRef<FString> SharedLineString = MakeShared<FString>(LineString);
			TArray<TSharedRef<IRun>> Runs;

			int32 WalkIdx = 0;
			while (WalkIdx < LineString.Len())
			{
				int32 OpenBraceIdx = LineString.Find(TEXT("{"), ESearchCase::CaseSensitive, ESearchDir::FromStart, WalkIdx);
				if (OpenBraceIdx == INDEX_NONE)
				{
					Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, NormalStyle, FTextRange(WalkIdx, LineString.Len())));
					break;
				}

				if (OpenBraceIdx > WalkIdx)
				{
					Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, NormalStyle, FTextRange(WalkIdx, OpenBraceIdx)));
				}

				int32 CloseBraceIdx = LineString.Find(TEXT("}"), ESearchCase::CaseSensitive, ESearchDir::FromStart, OpenBraceIdx);
				if (CloseBraceIdx != INDEX_NONE)
				{
					FString TagPart = LineString.Mid(OpenBraceIdx, CloseBraceIdx - OpenBraceIdx + 1);
					
					bool bIsValid = (TagPart == TEXT("{Objective}") || TagPart == TEXT("{POI}") || TagPart == TEXT("{POI_Desc}") || TagPart == TEXT("{POI_Loot}") || TagPart == TEXT("{Dir}"));
					
					Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, bIsValid ? ValidStyle : InvalidStyle, FTextRange(OpenBraceIdx, CloseBraceIdx + 1)));
					
					WalkIdx = CloseBraceIdx + 1;
				}
				else
				{
					Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, NormalStyle, FTextRange(OpenBraceIdx, LineString.Len())));
					break;
				}
			}

			if (Runs.Num() == 0)
			{
				Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, NormalStyle, FTextRange(0, 0)));
			}

			LinesToAdd.Add(FTextLayout::FNewLineData(SharedLineString, MoveTemp(Runs)));
			CurrentIdx = LineEndIdx + 1;
			if (CurrentIdx > SourceString.Len())
			{
				break;
			}
		}

		if (LinesToAdd.Num() == 0)
		{
			TSharedRef<FString> SharedLineString = MakeShared<FString>(TEXT(""));
			TArray<TSharedRef<IRun>> Runs;
			Runs.Add(FSlateTextRun::Create(FRunInfo(), SharedLineString, NormalStyle, FTextRange(0, 0)));
			LinesToAdd.Add(FTextLayout::FNewLineData(SharedLineString, MoveTemp(Runs)));
		}

		TargetTextLayout.AddLines(LinesToAdd);
	}
	
	virtual void GetText(FString& TargetString, const FTextLayout& SourceTextLayout) override
	{
		SourceTextLayout.GetAsText(TargetString);
	}
};

TSharedRef<IPropertyTypeCustomization> FFretteClueTemplateCustomization::MakeInstance()
{
	return MakeShareable(new FFretteClueTemplateCustomization());
}

void FFretteClueTemplateCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> ItemNameProperty = PropertyHandle->GetChildHandle(TEXT("ItemName"));
	
	if (ItemNameProperty.IsValid())
	{
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(300.f)
		[
			ItemNameProperty->CreatePropertyValueWidget()
		];
	}
	else
	{
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		];
	}
}

void FFretteClueTemplateCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ClueTextProperty = PropertyHandle->GetChildHandle(TEXT("ClueText"));

	auto OnTextChanged = [this](const FText& InText)
	{
		FString Str = InText.ToString();
		ClueTextProperty->SetValue(Str);
	};

	auto OnTextCommitted = [this](const FText& InText, ETextCommit::Type CommitType)
	{
		FString Str = InText.ToString();
		ClueTextProperty->SetValue(Str);
	};
	
	auto GetTextValue = [this]() -> FText
	{
		FString Val;
		if (ClueTextProperty && ClueTextProperty->GetValue(Val) == FPropertyAccess::Success)
		{
			return FText::FromString(Val);
		}
		if (ClueTextProperty)
		{
			FText TextVal;
			if (ClueTextProperty->GetValueAsFormattedText(TextVal) == FPropertyAccess::Success)
				return TextVal;
		}
		return FText::GetEmpty();
	};

	ChildBuilder.AddCustomRow(NSLOCTEXT("Frette", "ClueText", "Clue Text"))
	.WholeRowContent() // Fill the row with standard margin
	[
		SNew(SBox)
		.MinDesiredHeight(120.f)
		.Padding(FMargin(0, 5, 0, 5))
		[
			SNew(SMultiLineEditableTextBox)
			.Text_Lambda(GetTextValue)
			.OnTextChanged_Lambda(OnTextChanged)
			.OnTextCommitted_Lambda(OnTextCommitted)
			.Marshaller(FClueTextLayoutMarshaller::Create())
			.Font([]() {
				FSlateFontInfo FInfo = FAppStyle::Get().GetFontStyle("NormalFont");
				FInfo.Size = 16;
				return FInfo;
			}())
			.AllowContextMenu(true)
			.AutoWrapText(true)
		]
	];
}
