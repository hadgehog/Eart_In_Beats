#include "GestureHelper.h"

#include <string>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;

GestureHelper::GestureHelper(){
	this->recognizer = ref new GestureRecognizer();

	this->recognizer->GestureSettings = GestureSettings::ManipulationScale | GestureSettings::ManipulationTranslateX |
		GestureSettings::ManipulationTranslateY | GestureSettings::Tap | GestureSettings::ManipulationRotate;

	this->recognizer->Tapped += ref new TypedEventHandler<GestureRecognizer ^, TappedEventArgs ^>(this, &GestureHelper::OnTapped);
	this->recognizer->ManipulationStarted += ref new TypedEventHandler<GestureRecognizer ^, ManipulationStartedEventArgs ^>(this, &GestureHelper::OnManipulationStarted);
	this->recognizer->ManipulationCompleted += ref new TypedEventHandler<GestureRecognizer ^, ManipulationCompletedEventArgs ^>(this, &GestureHelper::OnManipulationCompleted);
	this->recognizer->ManipulationUpdated += ref new TypedEventHandler<GestureRecognizer ^, ManipulationUpdatedEventArgs ^>(this, &GestureHelper::OnManipulationUpdated);
}

void GestureHelper::ProcessPress(PointerPoint ^ppt){
	this->recognizer->ProcessDownEvent(ppt);
	if (this->Clicked) {
		this->Clicked(ppt->Position.X, ppt->Position.Y);
	}
}

void GestureHelper::ProcessMove(PointerPoint ^ppt){
	this->recognizer->ProcessMoveEvents(ppt->GetIntermediatePoints(ppt->PointerId));
}

void GestureHelper::ProcessRelease(PointerPoint ^ppt){
	this->recognizer->ProcessUpEvent(ppt);
}



void GestureHelper::OnManipulationStarted(GestureRecognizer^ sender, ManipulationStartedEventArgs^ e){
	if (this->Pressed){
		this->Pressed(e->Position.X, e->Position.Y);
	}
}

void GestureHelper::OnManipulationCompleted(GestureRecognizer ^sender, ManipulationCompletedEventArgs ^e) {
	if (this->Released) {
		this->Released(e->Position.X, e->Position.Y);
	}
}

void GestureHelper::OnManipulationUpdated(GestureRecognizer ^sender, ManipulationUpdatedEventArgs ^e){
	if (this->MoveUpdated){
		this->MoveUpdated(e->Position.X, e->Position.Y);
	}
	if (this->ZoomUpdated){
		this->ZoomUpdated(e->Delta.Scale);
	}
	if (this->RotateUpdated) {
		this->RotateUpdated(-e->Delta.Rotation);
	}
}

void GestureHelper::OnTapped(GestureRecognizer ^sender, TappedEventArgs ^e){
	if (this->Pressed) {
		this->Pressed(e->Position.X, e->Position.Y);
	}
	if (this->Released){
		this->Released(e->Position.X, e->Position.Y);
	}
}
