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
		GestureSettings::ManipulationTranslateY | GestureSettings::Tap | GestureSettings::ManipulationRotate | GestureSettings::DoubleTap;

	this->recognizer->Tapped += ref new TypedEventHandler<GestureRecognizer ^, TappedEventArgs ^>(this, &GestureHelper::OnTapped);
	this->recognizer->ManipulationStarted += ref new TypedEventHandler<GestureRecognizer ^, ManipulationStartedEventArgs ^>(this, &GestureHelper::OnManipulationStarted);
	this->recognizer->ManipulationCompleted += ref new TypedEventHandler<GestureRecognizer ^, ManipulationCompletedEventArgs ^>(this, &GestureHelper::OnManipulationCompleted);
	this->recognizer->ManipulationUpdated += ref new TypedEventHandler<GestureRecognizer ^, ManipulationUpdatedEventArgs ^>(this, &GestureHelper::OnManipulationUpdated);
	this->recognizer->ManipulationInertiaStarting += ref new TypedEventHandler<GestureRecognizer ^, ManipulationInertiaStartingEventArgs ^>(this, &GestureHelper::OnManipulationInertiaStarting);
}

void GestureHelper::ProcessPress(PointerPoint ^ppt){
	this->recognizer->ProcessDownEvent(ppt);
}

void GestureHelper::ProcessMove(PointerPoint ^ppt){
	this->recognizer->ProcessMoveEvents(ppt->GetIntermediatePoints(ppt->PointerId));
}

void GestureHelper::ProcessRelease(PointerPoint ^ppt){
	this->recognizer->ProcessUpEvent(ppt);
}

void GestureHelper::InitRecognizerInertia(){
	this->recognizer->ProcessInertia();
}


void GestureHelper::OnManipulationStarted(GestureRecognizer^ sender, ManipulationStartedEventArgs^ e){
	if (this->ManipulationStarted) {
		this->ManipulationStarted(e->Position.X, e->Position.Y);
	}
}

void GestureHelper::OnManipulationCompleted(GestureRecognizer ^sender, ManipulationCompletedEventArgs ^e) {
	if (this->ManipulationCompleted) {
		DirectX::XMFLOAT2 pos(e->Position.X, e->Position.Y);

		this->ManipulationCompleted(pos);
	}
}

void GestureHelper::OnManipulationUpdated(GestureRecognizer ^sender, ManipulationUpdatedEventArgs ^e){
	if (this->MoveUpdated) {
		DirectX::XMFLOAT2 moveVec(e->Delta.Translation.X, e->Delta.Translation.Y);
		DirectX::XMFLOAT2 newPos(e->Position.X, e->Position.Y);

		this->MoveUpdated(moveVec, newPos);
	}

	if (this->ZoomUpdated && !DirectX::XMScalarNearEqual(e->Delta.Scale, 1.0f, FLT_EPSILON)) {
		this->ZoomUpdated(e->Delta.Scale, e->Position.X, e->Position.Y);
	}

	if (this->RotateUpdated) {
		this->RotateUpdated(e->Delta.Rotation, e->Position.X, e->Position.Y);
	}
}

void GestureHelper::OnTapped(GestureRecognizer ^sender, TappedEventArgs ^e){
	if (this->Tapped) {
		this->Tapped(e->TapCount, e->Position.X, e->Position.Y);
	}
}

void GestureHelper::OnManipulationInertiaStarting(GestureRecognizer ^sender, ManipulationInertiaStartingEventArgs ^e) {
	if (this->MoveUpdated) {
		DirectX::XMFLOAT2 moveVec(e->Delta.Translation.X, e->Delta.Translation.Y);
		DirectX::XMFLOAT2 newPos(e->Position.X, e->Position.Y);

		sender->InertiaTranslationDeceleration = 0.02f;
		this->MoveUpdated(moveVec, newPos);
	}
}
